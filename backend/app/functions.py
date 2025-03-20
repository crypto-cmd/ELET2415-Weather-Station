#!/usr/bin/python3


#################################################################################################################################################
#                                                    CLASSES CONTAINING ALL THE APP FUNCTIONS                                                                                                    #
#################################################################################################################################################


class DB:

    def __init__(self, Config):

        from math import floor
        from os import getcwd
        from os.path import join
        from json import loads, dumps, dump
        from datetime import timedelta, datetime, timezone
        from pymongo import MongoClient, errors, ReturnDocument
        from urllib import parse
        from urllib.request import urlopen
        from bson.objectid import ObjectId

        self.Config = Config
        self.getcwd = getcwd
        self.join = join
        self.floor = floor
        self.loads = loads
        self.dumps = dumps
        self.dump = dump
        self.datetime = datetime
        self.ObjectId = ObjectId
        self.server = Config.DB_SERVER
        self.port = Config.DB_PORT
        self.username = parse.quote_plus(Config.DB_USERNAME)
        self.password = parse.quote_plus(Config.DB_PASSWORD)
        self.remoteMongo = MongoClient
        self.ReturnDocument = ReturnDocument
        self.PyMongoError = errors.PyMongoError
        self.BulkWriteError = errors.BulkWriteError
        self.tls = False  # MUST SET TO TRUE IN PRODUCTION

    def __del__(self):
        # Delete class instance to free resources
        pass


# 1. CREATE FUNCTION TO INSERT DATA IN TO THE WEATHER COLLECTION


    def insertData(self, data):
        ''' Insert data into the weather collection '''
        try:
            # Insert data into the weather collection

            remotedb = self.remoteMongo(
                'mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            result = remotedb.ELET2415.weather.insert_one(data)
        except Exception as e:
            msg = str(e)
            print("insertData Error: ", msg)
        else:
            return result

# 2. CREATE FUNCTION TO RETRIEVE THE LATEST DATA FROM THE WEATHER COLLECTION

    def retrieve_latest(self):
        ''' Retrieve the latest data from the weather collection '''
        try:
            # Retrieve the latest data from the weather collection
            remotedb = self.remoteMongo(
                'mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            result = remotedb.ELET2415.weather.find_one(
                {},
                {"_id": 0},
                sort=[('timestamp', -1)]
            )
            print(result)
        except Exception as e:
            print(f"retrieve_sense() error: {e}")
            return False
        else:
            return result
# 3. CREATE FUNCTION TO RETRIEVE THE DATA FROM THE WEATHER COLLECTION BETWEEN TWO DATES

    def retrieve_sense_dates(self, start, end):
        ''' Retrieve the data from the weather collection between two dates '''
        try:
            # Retrieve the data from the weather collection between two dates
            start = int(start)
            end = int(end)

            remotedb = self.remoteMongo(
                'mongodb://%s:%s@%s:%s' % (self.username, self.password, self.server, self.port), tls=self.tls)
            result = remotedb.ELET2415.weather.find({"timestamp": {"$gte": start, "$lte": end}, "id": {
                "$exists": True}}, {"_id": 0, "timestamp": 1,
                                    "temperature": 1, "humidity": 1, "pressure": 1, "altitude": 1, "moisture": 1, "heatIndex": 1})
        except Exception as e:
            print(f"retrieve_sense_dates() error: {e}")
            return False
        else:
            return result


def main():
    from backend.app.config import Config
    from time import time, ctime, sleep
    from math import floor
    from datetime import datetime, timedelta
    one = DB(Config)

    start = time()
    end = time()
    print(f"completed in: {end - start} seconds")


if __name__ == '__main__':
    main()
