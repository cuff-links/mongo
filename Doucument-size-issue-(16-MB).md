I am using an query using aggregation on a collection inside which i am doing two lookups every thing is working fine until i query for a large number of records around l lakh record, when i am querying the document size  of the result is getting higher then 16MB. so i am not able to query. below is the query which i am actually doing


`  {"collectionName": "UserOrg",`
  `"runCommand": {`
    `"aggregate": "UserOrg",`
    `"pipeline": [`
      `{`
        `"$lookup": {`
          `"from": "Org",`
          `"localField": "orgId",`
          `"foreignField": "_id",`
          `"as": "org"`
        `}`
      `},`
      `{`
        `"$lookup": {`
          `"from": "user",`
          `"localField": "user_id",`
          `"foreignField": "_id",`
          `"as": "user"`
        `}`
      `}`
    `]`
  `}}`


so anybody can help me with maximum document size issue, i really don't understand why people has created criteria itself.