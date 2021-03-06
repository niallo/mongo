// grid.h

/**
*    Copyright (C) 2010 10gen Inc.
*
*    This program is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "../util/concurrency/mutex.h"

#include "config.h"  // DBConfigPtr

namespace mongo {

    /**
     * stores meta-information about the grid
     * TODO: used shard_ptr for DBConfig pointers
     */
    class Grid {
    public:
        Grid() : _lock( "Grid" ) , _allowLocalShard( true ) { }

        /**
         * gets the config the db.
         * will return an empty DBConfig if not in db already
         */
        DBConfigPtr getDBConfig( string ns , bool create=true , const string& shardNameHint="" );
        
        /**
         * removes db entry.
         * on next getDBConfig call will fetch from db
         */
        void removeDB( string db );

        /**
         * @return true if shards and config servers are allowed to use 'localhost' in address
         */
        bool allowLocalHost() const;

        /**
         * @param whether to allow shards and config servers to use 'localhost' in address
         */
        void setAllowLocalHost( bool allow );

        /**
         *
         * addShard will create a new shard in the grid. It expects a mongod process to be runing
         * on the provided address.
         * TODO - add the mongod's databases to the grid
         *
         * @param name is an optional string with the name of the shard. if ommited, grid will
         * generate one and update the parameter.
         * @param host is the complete address of the machine where the shard will be
         * @param maxSize is the optional space quota in bytes. Zeros means there's no limitation to
         * space usage
         * @param errMsg is the error description in case the operation failed. 
         * @return true if shard was successfully added.
         */
        bool addShard( string* name , const string& host , long long maxSize , string& errMsg );

        /**
         * @return true if the config database knows about a host 'name'
         */
        bool knowAboutShard( const string& name ) const;
        
        /**
         * @return true if the chunk balancing functionality is enabled
         */
        bool shouldBalance() const;

        unsigned long long getNextOpTime() const;

    private:
        mongo::mutex              _lock;            // protects _databases; TODO: change to r/w lock ??
        map<string, DBConfigPtr > _databases;       // maps ns to DBConfig's
        bool                      _allowLocalShard; // can 'localhost' be used in shard addresses?

        /**
         * @param name is the chose name for the shard. Parameter is mandatory.
         * @return true if it managed to generate a shard name. May return false if (currently)
         * 10000 shard 
         */
        bool _getNewShardName( string* name ) const;

        /**
         * @return whether a give dbname is used for shard "local" databases (e.g., admin or local)
         */
        static bool _isSpecialLocalDB( const string& dbName );

    };

    extern Grid grid;

} // namespace mongo
