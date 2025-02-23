/**
 *    Copyright (C) 2023-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */


#include <boost/cstdint.hpp>
#include <boost/optional/optional.hpp>

#include "mongo/base/error_codes.h"
#include "mongo/base/status.h"
#include "mongo/db/auth/authorization_session.h"
#include "mongo/db/commands.h"
#include "mongo/db/commands/auto_compact.h"
#include "mongo/db/commands/compact_gen.h"
#include "mongo/db/concurrency/d_concurrency.h"
#include "mongo/logv2/log.h"
#include "mongo/logv2/log_attr.h"
#include "mongo/logv2/log_component.h"

#define MONGO_LOGV2_DEFAULT_COMPONENT ::mongo::logv2::LogComponent::kCommand

namespace mongo {

class AutoCompactCmd final : public TypedCommand<AutoCompactCmd> {
public:
    using Request = AutoCompact;

    class Invocation final : public InvocationBase {
    public:
        using InvocationBase::InvocationBase;

        void typedRun(OperationContext* opCtx) {
            uassertStatusOK(autoCompact(
                opCtx, request().getCommandParameter(), request().getFreeSpaceTargetMB()));
        }

    private:
        bool supportsWriteConcern() const override {
            return false;
        }

        void doCheckAuthorization(OperationContext* opCtx) const override {
            auto* as = AuthorizationSession::get(opCtx->getClient());
            uassert(ErrorCodes::Unauthorized,
                    "Not authorized",
                    as->isAuthorizedForActionsOnResource(
                        ResourcePattern::forClusterResource(request().getDbName().tenantId()),
                        ActionType::compact));
        }

        NamespaceString ns() const override {
            return NamespaceString::kEmpty;
        }
    };

    bool adminOnly() const override {
        return true;
    }

    AllowedOnSecondary secondaryAllowed(ServiceContext*) const override {
        return AllowedOnSecondary::kAlways;
    }

    std::string help() const override {
        return "enable auto compaction for a database\n"
               "warning: compact operation has blocking behaviour and is slow, enabling auto "
               "compact will allow compact to run on any collection at any time. You can cancel "
               "by disabling auto compact.\n"
               "{ autoCompact : <bool>, [force:<bool>], [freeSpaceTargetMB:<int64_t>] }\n"
               "  force - allows to run on a replica set primary\n";
    }
};

MONGO_REGISTER_COMMAND(AutoCompactCmd).forShard();

Status autoCompact(OperationContext* opCtx,
                   bool enable,
                   boost::optional<int64_t> freeSpaceTargetMB) {

    auto* storageEngine = opCtx->getServiceContext()->getStorageEngine();

    Lock::GlobalLock lk(opCtx, MODE_IX);

    Status status = storageEngine->autoCompact(opCtx, enable, freeSpaceTargetMB);
    if (!status.isOK())
        return status;

    if (enable)
        LOGV2(8012100, "AutoCompact enabled");
    else
        LOGV2(8012101, "AutoCompact disabled");

    return status;
}

}  // namespace mongo
