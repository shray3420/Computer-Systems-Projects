# 
# This file was created as part of CS 340's Spring 2022 Final Project
# - This file was further modified for MP9.
# - It should not need to be modified further for you to complete MP9 -- you need only focus on the MGs! :)
#


USE_MONGO_DB = True





if USE_MONGO_DB:
    from pymongo import MongoClient
    from bson.objectid import ObjectId

    HOST = 'localhost'
    PORT = 27017
    DB_NAME = 'cs240-infinite-maze'
    server_keys = ['name', 'url', 'author', 'weight']

    class Connection:
        def __init__(self, host=HOST, port=PORT, db_name=DB_NAME):
            self.mongo = MongoClient(host, port)
            self.db = self.mongo[db_name]

        def __del__(self):
            if self.mongo:
                self.mongo.close()

        def stringify_id(document):
            document["_id"] = str(document["_id"])
            return document

        def add_server(self, data):
            return self.db.servers.insert_one(data)

        def update_server(self, id, data):
            return self.db.servers.update_one({"_id": ObjectId(id)}, {"$set": data})

        def remove_server(self, id):
            return self.db.servers.delete_one({"_id": ObjectId(id)})

        def get_server(self, name):
            found = self.db.servers.find_one({"name": name})

            if found:
                found = Connection.stringify_id(found)
                return found

            return None

        def get_all_servers(self):
            result = self.db.servers.find({})

            if result:
                result = list(result)
                for server in result:
                    server = Connection.stringify_id(server)
                return result

            return []

else:
    uid = 0
    class Connection:
        def __init__(self):
            self.d = {}

        def stringify_id(document):
            return document

        def add_server(self, data):
            global uid
            data["_id"] = uid
            self.d[uid] = data
            uid += 1
            return data

        def update_server(self, id, data):
            self.d[id] = data
            return data

        def remove_server(self, id):
            del self.d[id]

        def get_server(self, name):
            for k in self.d:
                v = self.d[k]
                if v["name"] == name:
                    return v
            
            return None

        def get_all_servers(self):
            result = []

            for k in self.d:
                v = self.d[k]
                result.append(v)

            return result

