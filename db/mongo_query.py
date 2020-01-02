import pymongo

host = "39.97.251.94:27017"
dbname="pm25"
username = "yur"
password = "password"
client = pymongo.MongoClient(host)
client.the_database.authenticate(username, password, source=dbname)
db = client[dbname]

print(db.list_collection_names())