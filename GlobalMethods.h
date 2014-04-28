/*
* Copyright (C) 2010 - 2014 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.md for more information
*/

#ifndef GLOBALMETHODS_H
#define GLOBALMETHODS_H

namespace LuaGlobalFunctions
{
    /* GETTERS */
    int GetLuaEngine(lua_State* L)
    {
        Eluna::Push(L, "ElunaEngine");
        return 1;
    }

    int GetCoreName(lua_State* L)
    {
        Eluna::Push(L, CORE_NAME);
        return 1;
    }

    int GetCoreVersion(lua_State* L)
    {
        Eluna::Push(L, CORE_VERSION);
        return 1;
    }

    int GetCoreExpansion(lua_State* L)
    {
#ifdef CLASSIC
        Eluna::Push(L, 0);
#elif defined(TBC)
        Eluna::Push(L, 1);
#elif defined(WOTLK)
        Eluna::Push(L, 2);
#elif defined(CATA)
        Eluna::Push(L, 3);
#else
        Eluna::Push(L, );
#endif
        return 1;
    }

    int GetQuest(lua_State* L)
    {
        uint32 questId = Eluna::CHECKVAL<uint32>(L, 1);

        Eluna::Push(L, sObjectMgr->GetQuestTemplate(questId));
        return 1;
    }

    int GetPlayerByGUID(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);
        Eluna::Push(L, sObjectAccessor->FindPlayer(ObjectGuid(guid)));
        return 1;
    }

    int GetPlayerByName(lua_State* L)
    {
        const char* message = Eluna::CHECKVAL<const char*>(L, 1);
        Eluna::Push(L, sObjectAccessor->FindPlayerByName(message));
        return 1;
    }

    int GetGameTime(lua_State* L)
    {
        time_t time = sWorld->GetGameTime();
        if (time < 0)
            Eluna::Push(L, int32(time));
        else
            Eluna::Push(L, uint32(time));
        return 1;
    }

    int GetPlayersInMap(lua_State* L)
    {
        uint32 mapID = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 instanceID = Eluna::CHECKVAL<uint32>(L, 2, 0);
        uint32 team = Eluna::CHECKVAL<uint32>(L, 3, TEAM_NEUTRAL);

        Map* map = sMapMgr->FindMap(mapID, instanceID);
        if (!map)
            return 0;

        lua_newtable(L);
        int tbl = lua_gettop(L);
        uint32 i = 0;

        Map::PlayerList const& players = map->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
#ifdef MANGOS
            Player* player = itr->getSource();
#else
            Player* player = itr->GetSource();
#endif
            if (!player)
                continue;
            if (player->GetSession() && (team >= TEAM_NEUTRAL || player->GetTeamId() == team))
            {
                ++i;
                Eluna::Push(L, i);
                Eluna::Push(L, player);
                lua_settable(L, tbl);
            }
        }

        lua_settop(L, tbl);
        return 1;
    }

    int GetGuildByName(lua_State* L)
    {
        const char* name = Eluna::CHECKVAL<const char*>(L, 1);
        Eluna::Push(L, sGuildMgr->GetGuildByName(name));
        return 1;
    }

    int GetMap(lua_State* L)
    {
        Eluna* E = Eluna::GetEluna(L);
        if (E)
            Eluna::Push(L, E->GMap);
        return 1;
    }

    int GetGuildByLeaderGUID(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);

        Eluna::Push(L, sGuildMgr->GetGuildByLeader(ObjectGuid(guid)));
        return 1;
    }

    int GetPlayerCount(lua_State* L)
    {
        Eluna::Push(L, sWorld->GetActiveSessionCount());
        return 1;
    }

    int GetPlayerGUID(lua_State* L)
    {
        uint32 lowguid = Eluna::CHECKVAL<uint32>(L, 1);
        Eluna::Push(L, MAKE_NEW_GUID(lowguid, 0, HIGHGUID_PLAYER));
        return 1;
    }

    int GetItemGUID(lua_State* L)
    {
        uint32 lowguid = Eluna::CHECKVAL<uint32>(L, 1);
        Eluna::Push(L, MAKE_NEW_GUID(lowguid, 0, HIGHGUID_ITEM));
        return 1;
    }

    int GetObjectGUID(lua_State* L)
    {
        uint32 lowguid = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, MAKE_NEW_GUID(lowguid, entry, HIGHGUID_GAMEOBJECT));
        return 1;
    }

    int GetUnitGUID(lua_State* L)
    {
        uint32 lowguid = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, MAKE_NEW_GUID(lowguid, entry, HIGHGUID_UNIT));
        return 1;
    }

    int GetGUIDLow(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);

        Eluna::Push(L, GUID_LOPART(guid));
        return 1;
    }

    int GetItemLink(lua_State* L)
    {
        /*
        LOCALE_enUS = 0,
        LOCALE_koKR = 1,
        LOCALE_frFR = 2,
        LOCALE_deDE = 3,
        LOCALE_zhCN = 4,
        LOCALE_zhTW = 5,
        LOCALE_esES = 6,
        LOCALE_esMX = 7,
        LOCALE_ruRU = 8
        */
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        int loc_idx = Eluna::CHECKVAL<int>(L, 2, DEFAULT_LOCALE);
        if (loc_idx < 0 || loc_idx >= MAX_LOCALES)
            return luaL_argerror(L, 2, "valid LocaleConstant expected");

        const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
        if (!temp)
            return luaL_argerror(L, 1, "valid ItemEntry expected");

        std::string name = temp->Name1;
        if (ItemLocale const* il = sObjectMgr->GetItemLocale(entry))
            ObjectMgr::GetLocaleString(il->Name, loc_idx, name);

        std::ostringstream oss;
        oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
            "|Hitem:" << entry << ":0:" <<
#ifndef CLASSIC
            "0:0:0:0:" <<
#endif
            "0:0:0:0|h[" << name << "]|h|r";

        Eluna::Push(L, oss.str());
        return 1;
    }

    int GetGUIDType(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);
        Eluna::Push(L, GUID_HIPART(guid));
        return 1;
    }

    int GetGUIDEntry(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);
        Eluna::Push(L, GUID_ENPART(guid));
        return 1;
    }

    /* OTHER */
    int RegisterPacketEvent(lua_State* L)
    {
        lua_settop(L, 2);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 1);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_PACKET, 0, ev, functionRef);
        return 0;
    }

    int RegisterServerEvent(lua_State* L)
    {
        lua_settop(L, 2);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 1);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_SERVER, 0, ev, functionRef);
        return 0;
    }

    int RegisterPlayerEvent(lua_State* L)
    {
        lua_settop(L, 2);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 1);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_PLAYER, 0, ev, functionRef);
        return 0;
    }

    int RegisterGuildEvent(lua_State* L)
    {
        lua_settop(L, 2);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 1);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_GUILD, 0, ev, functionRef);
        return 0;
    }

    int RegisterGroupEvent(lua_State* L)
    {
        lua_settop(L, 2);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 1);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_GROUP, 0, ev, functionRef);
        return 0;
    }

    int RegisterCreatureGossipEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_CREATURE_GOSSIP, entry, ev, functionRef);
        return 0;
    }

    int RegisterGameObjectGossipEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_GAMEOBJECT_GOSSIP, entry, ev, functionRef);
        return 0;
    }

    int RegisterItemEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_ITEM, entry, ev, functionRef);
        return 0;
    }

    int RegisterItemGossipEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_ITEM_GOSSIP, entry, ev, functionRef);
        return 0;
    }

    int RegisterPlayerGossipEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 menu_id = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_PLAYER_GOSSIP, menu_id, ev, functionRef);
        return 0;
    }

    int RegisterCreatureEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_CREATURE, entry, ev, functionRef);
        return 0;
    }

    int RegisterGameObjectEvent(lua_State* L)
    {
        lua_settop(L, 3);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 ev = Eluna::CHECKVAL<uint32>(L, 2);
        luaL_checktype(L, lua_gettop(L), LUA_TFUNCTION);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        if (functionRef > 0)
            Eluna::GetEluna(L)->Register(REGTYPE_GAMEOBJECT, entry, ev, functionRef);
        return 0;
    }

    int SendWorldMessage(lua_State* L)
    {
        const char* message = Eluna::CHECKVAL<const char*>(L, 1);
        sWorld->SendServerMessage(SERVER_MSG_STRING, message);
        return 0;
    }

    int WorldDBQuery(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);
        if (!query)
            return 0;

        QueryResult* result = NULL;
#ifdef MANGOS
        result = WorldDatabase.Query(query);
#else
        QueryResult res = WorldDatabase.Query(query);
        if (res)
            result = new QueryResult(res);
#endif
        if (!result)
            return 0;

        Eluna::Push(L, result);
        return 1;
    }

    int WorldDBExecute(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);
        WorldDatabase.Execute(query);
        return 0;
    }

    int CharDBQuery(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);

        QueryResult* result = NULL;
#ifdef MANGOS
        result = CharacterDatabase.Query(query);
#else
        QueryResult res = CharacterDatabase.Query(query);
        if (res)
            result = new QueryResult(res);
#endif
        if (!result)
            return 0;

        Eluna::Push(L, result);
        return 1;
    }

    int CharDBExecute(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);
        CharacterDatabase.Execute(query);
        return 0;
    }

    int AuthDBQuery(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);

        QueryResult* result = NULL;
#ifdef MANGOS
        result = LoginDatabase.Query(query);
#else
        QueryResult res = LoginDatabase.Query(query);
        if (res)
            result = new QueryResult(res);
#endif
        if (!result)
            return 0;

        Eluna::Push(L, result);
        return 1;
    }

    int AuthDBExecute(lua_State* L)
    {
        const char* query = Eluna::CHECKVAL<const char*>(L, 1);
        LoginDatabase.Execute(query);
        return 0;
    }

    int CreateLuaEvent(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TFUNCTION);
        uint32 delay = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 repeats = Eluna::CHECKVAL<uint32>(L, 3);

        Eluna* E = Eluna::GetEluna(L);
        if (!E)
            return 1;

        lua_settop(L, 1);
        int functionRef = luaL_ref(L, LUA_REGISTRYINDEX);
        functionRef = E->m_EventMgr.AddEvent(E, functionRef, delay, repeats, &E->m_EventMgr.GlobalEvents);
        if (functionRef)
            Eluna::Push(L, functionRef);
        else
            Eluna::Push(L);
        return 1;
    }

    int RemoveEventById(lua_State* L)
    {
        int eventId = Eluna::CHECKVAL<int>(L, 1);
        bool all_Events = Eluna::CHECKVAL<bool>(L, 1, false);

        Eluna* E = Eluna::GetEluna(L);
        if (!E)
            return 0;

        if (all_Events)
            E->m_EventMgr.RemoveEvent(eventId);
        else
            E->m_EventMgr.RemoveEvent(&E->m_EventMgr.GlobalEvents, eventId);
        return 0;
    }

    int RemoveEvents(lua_State* L)
    {
        bool all_Events = Eluna::CHECKVAL<bool>(L, 1, false);

        Eluna* E = Eluna::GetEluna(L);
        if (!E)
            return 0;

        if (all_Events)
            E->m_EventMgr.RemoveEvents();
        else
            E->m_EventMgr.GlobalEvents.KillAllEvents(true);
        return 0;
    }

    int PerformIngameSpawn(lua_State* L)
    {
        int spawntype = Eluna::CHECKVAL<int>(L, 1);
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 mapID = Eluna::CHECKVAL<uint32>(L, 3);
        uint32 instanceID = Eluna::CHECKVAL<uint32>(L, 4);
        float x = Eluna::CHECKVAL<float>(L, 5);
        float y = Eluna::CHECKVAL<float>(L, 6);
        float z = Eluna::CHECKVAL<float>(L, 7);
        float o = Eluna::CHECKVAL<float>(L, 8);
        bool save = Eluna::CHECKVAL<bool>(L, 9, false);
        uint32 durorresptime = Eluna::CHECKVAL<uint32>(L, 10, 0);
#if (!defined(TBC) && !defined(CLASSIC))
        uint32 phase = Eluna::CHECKVAL<uint32>(L, 11, PHASEMASK_NORMAL);
        if (!phase)
            return 0;
#endif

#ifdef MANGOS
        Map* map = sMapMgr->FindMap(mapID, instanceID);
        if (!map)
            return 0;

        if (spawntype == 1) // spawn creature
        {
            if (save)
            {
                CreatureInfo const* cinfo = ObjectMgr::GetCreatureTemplate(entry);
                if (!cinfo)
                    return 0;

#if (defined(TBC) || defined(CLASSIC))
                CreatureCreatePos pos(map, x, y, z, o);
#else
                CreatureCreatePos pos(map, x, y, z, o, phase);
#endif
                Creature* pCreature = new Creature;
                // used guids from specially reserved range (can be 0 if no free values)
                uint32 lowguid = sObjectMgr->GenerateStaticCreatureLowGuid();
                if (!lowguid)
                    return 0;

                if (!pCreature->Create(lowguid, pos, cinfo))
                {
                    delete pCreature;
                    return 0;
                }

#ifdef TBC
                pCreature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()));
#elif defined(CLASSIC)
                pCreature->SaveToDB(map->GetId());
#else
                pCreature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), phase);
#endif

                uint32 db_guid = pCreature->GetGUIDLow();

                // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
                pCreature->LoadFromDB(db_guid, map);

                map->Add(pCreature);
                sObjectMgr->AddCreatureToGrid(db_guid, sObjectMgr->GetCreatureData(db_guid));
                if (durorresptime)
                    pCreature->ForcedDespawn(durorresptime);

                Eluna::Push(L, pCreature);
            }
            else
            {
                CreatureInfo const* cinfo = ObjectMgr::GetCreatureTemplate(entry);
                if (!cinfo)
                    return 0;

                TemporarySummon* pCreature = new TemporarySummon(ObjectGuid(uint64(0)));
#if (defined(TBC) || defined(CLASSIC))
                CreatureCreatePos pos(map, x, y, z, o);
#else
                CreatureCreatePos pos(map, x, y, z, o, phase);
#endif

                if (!pCreature->Create(map->GenerateLocalLowGuid(cinfo->GetHighGuid()), pos, cinfo, TEAM_NONE))
                {
                    delete pCreature;
                    return NULL;
                }

                pCreature->SetRespawnCoord(pos);

                // Active state set before added to map
                pCreature->SetActiveObjectState(false);

                // Also initializes the AI and MMGen
                pCreature->Summon(durorresptime ? TEMPSUMMON_TIMED_OR_DEAD_DESPAWN : TEMPSUMMON_MANUAL_DESPAWN, durorresptime);

                // Creature Linking, Initial load is handled like respawn
                if (pCreature->IsLinkingEventTrigger())
                    map->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, pCreature);

                Eluna::Push(L, pCreature);
            }

            return 1;
        }

        if (spawntype == 2) // Spawn object
        {
            if (save)
            {
                const GameObjectInfo* gInfo = ObjectMgr::GetGameObjectInfo(entry);
                if (!gInfo)
                    return 0;

                // used guids from specially reserved range (can be 0 if no free values)
                uint32 db_lowGUID = sObjectMgr->GenerateStaticGameObjectLowGuid();
                if (!db_lowGUID)
                    return 0;

                GameObject* pGameObj = new GameObject;
#if (defined(TBC) || defined(CLASSIC))
                if (!pGameObj->Create(db_lowGUID, gInfo->id, map, x, y, z, o))
#else
                if (!pGameObj->Create(db_lowGUID, gInfo->id, map, phase, x, y, z, o))
#endif
                {
                    delete pGameObj;
                    return 0;
                }

                if (durorresptime)
                    pGameObj->SetRespawnTime(durorresptime);

                // fill the gameobject data and save to the db
#ifdef TBC
                pGameObj->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()));
#elif defined(CLASSIC)
                pGameObj->SaveToDB(map->GetId());
#else
                pGameObj->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), phase);
#endif

                // this will generate a new guid if the object is in an instance
                if (!pGameObj->LoadFromDB(db_lowGUID, map))
                {
                    delete pGameObj;
                    return false;
                }

                // DEBUG_LOG(GetMangosString(LANG_GAMEOBJECT_CURRENT), gInfo->name, db_lowGUID, x, y, z, o);

                map->Add(pGameObj);

                sObjectMgr->AddGameobjectToGrid(db_lowGUID, sObjectMgr->GetGOData(db_lowGUID));

                Eluna::Push(L, pGameObj);
            }
            else
            {
                GameObject* pGameObj = new GameObject;

#if (defined(TBC) || defined(CLASSIC))
                if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), entry, map, x, y, z, o))
#else
                if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), entry, map, phase, x, y, z, o))
#endif
                {
                    delete pGameObj;
                    return NULL;
                }

                pGameObj->SetRespawnTime(durorresptime / IN_MILLISECONDS);

                map->Add(pGameObj);

                Eluna::Push(L, pGameObj);
            }
            return 1;
        }
#else
        Map* map = sMapMgr->FindMap(mapID, instanceID);
        if (!map)
            return 0;

        Position pos = { x, y, z, o };

        if (spawntype == 1) // spawn creature
        {
            if (save)
            {
                Creature* creature = new Creature();
                if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT), map, phase, entry, 0, 0, x, y, z, o))
                {
                    delete creature;
                    return 0;
                }

                creature->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), phase);

                uint32 db_lowguid = creature->GetDBTableGUIDLow();
                if (!creature->LoadCreatureFromDB(db_lowguid, map))
                {
                    delete creature;
                    return 0;
                }

                sObjectMgr->AddCreatureToGrid(db_lowguid, sObjectMgr->GetCreatureData(db_lowguid));
                Eluna::Push(L, creature);
            }
            else
            {
                TempSummon* creature = map->SummonCreature(entry, pos, NULL, durorresptime);
                if (!creature)
                    return 0;

                if (durorresptime)
                    creature->SetTempSummonType(TEMPSUMMON_TIMED_OR_DEAD_DESPAWN);
                else
                    creature->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);

                Eluna::Push(L, creature);
            }

            return 1;
        }

        if (spawntype == 2) // Spawn object
        {
            const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(entry);
            if (!objectInfo)
                return 0;

            if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
                return 0;

            GameObject* object = new GameObject;
            uint32 lowguid = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

            if (!object->Create(lowguid, objectInfo->entry, map, phase, x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, 0, GO_STATE_READY))
            {
                delete object;
                return 0;
            }

            if (durorresptime)
                object->SetRespawnTime(durorresptime);

            if (save)
            {
                // fill the gameobject data and save to the db
                object->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), phase);

                // this will generate a new lowguid if the object is in an instance
                if (!object->LoadGameObjectFromDB(lowguid, map))
                {
                    delete object;
                    return false;
                }

                sObjectMgr->AddGameobjectToGrid(lowguid, sObjectMgr->GetGOData(lowguid));
            }
            else
                map->AddToMap(object);
            Eluna::Push(L, object);
            return 1;
        }
#endif
        return 0;
    }

    // CreatePacket(opcode, size)
    int CreatePacket(lua_State* L)
    {
        uint32 opcode = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 size = Eluna::CHECKVAL<uint32>(L, 2);
        if (opcode >= NUM_MSG_TYPES)
            return luaL_argerror(L, 1, "valid opcode expected");

        Eluna::Push(L, new WorldPacket((Opcodes)opcode, size));
        return 1;
    }

    int AddVendorItem(lua_State* L)
    {
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 item = Eluna::CHECKVAL<uint32>(L, 2);
        int maxcount = Eluna::CHECKVAL<int>(L, 3);
        uint32 incrtime = Eluna::CHECKVAL<uint32>(L, 4);
        uint32 extendedcost = Eluna::CHECKVAL<uint32>(L, 5);

#ifdef MANGOS
        if (!sObjectMgr->IsVendorItemValid(false, "npc_vendor", entry, item, maxcount, incrtime, extendedcost, 0))
            return 0;
#ifndef CLASSIC
        sObjectMgr->AddVendorItem(entry, item, maxcount, incrtime, extendedcost);
#else
        sObjectMgr->AddVendorItem(entry, item, maxcount, incrtime);
#endif
#else
#ifdef CATA
        if (!sObjectMgr->IsVendorItemValid(entry, item, maxcount, incrtime, extendedcost, 1))
            return 0;
        sObjectMgr->AddVendorItem(entry, item, maxcount, incrtime, extendedcost, 1);
#else
        if (!sObjectMgr->IsVendorItemValid(entry, item, maxcount, incrtime, extendedcost))
            return 0;
        sObjectMgr->AddVendorItem(entry, item, maxcount, incrtime, extendedcost);
#endif
#endif
        return 0;
    }

    int VendorRemoveItem(lua_State* L)
    {
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 item = Eluna::CHECKVAL<uint32>(L, 2);
        if (!sObjectMgr->GetCreatureTemplate(entry))
            return luaL_argerror(L, 1, "valid CreatureEntry expected");

#ifdef CATA
        sObjectMgr->RemoveVendorItem(entry, item, 1);
#else
        sObjectMgr->RemoveVendorItem(entry, item);
#endif
        return 0;
    }

    int VendorRemoveAllItems(lua_State* L)
    {
        uint32 entry = Eluna::CHECKVAL<uint32>(L, 1);

        VendorItemData const* items = sObjectMgr->GetNpcVendorItemList(entry);
        if (!items || items->Empty())
            return 0;

        VendorItemList const itemlist = items->m_items;
        for (VendorItemList::const_iterator itr = itemlist.begin(); itr != itemlist.end(); ++itr)
#ifdef CATA
            sObjectMgr->RemoveVendorItem(entry, (*itr)->item, 1);
#else
            sObjectMgr->RemoveVendorItem(entry, (*itr)->item);
#endif
        return 0;
    }

    int Kick(lua_State* L)
    {
        Player* player = Eluna::CHECKOBJ<Player>(L, 1);
        player->GetSession()->KickPlayer();
        return 0;
    }

    int Ban(lua_State* L)
    {
        int banMode = Eluna::CHECKVAL<int>(L, 1);
        const char* nameOrIP_cstr = Eluna::CHECKVAL<const char*>(L, 2);
        uint32 duration = Eluna::CHECKVAL<uint32>(L, 3);
        const char* reason = Eluna::CHECKVAL<const char*>(L, 4);
        Player* whoBanned = Eluna::CHECKOBJ<Player>(L, 5);
        std::string nameOrIP(nameOrIP_cstr);

        switch (banMode)
        {
        case BAN_ACCOUNT:
            if (!AccountMgr::normalizeString(nameOrIP))
                return 0;
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
                return 0;
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return 0;
            break;
        default:
            return 0;
        }

        switch (sWorld->BanAccount((BanMode)banMode, nameOrIP, duration, reason, whoBanned->GetSession() ? whoBanned->GetName() : ""))
        {
        case BAN_SUCCESS:
            if (duration > 0)
                ChatHandler(whoBanned->GetSession()).PSendSysMessage(LANG_BAN_YOUBANNED, nameOrIP.c_str(), secsToTimeString(duration, true).c_str(), reason);
            else
                ChatHandler(whoBanned->GetSession()).PSendSysMessage(LANG_BAN_YOUPERMBANNED, nameOrIP.c_str(), reason);
            break;
        case BAN_SYNTAX_ERROR:
            return 0;
        case BAN_NOTFOUND:
            return 0;
        }
        return 0;
    }

    int SaveAllPlayers(lua_State* L)
    {
        sObjectAccessor->SaveAllPlayers();
        return 0;
    }

    int SendMail(lua_State* L)
    {
        int i = 0;
        std::string subject = Eluna::CHECKVAL<std::string>(L, ++i);
        std::string text = Eluna::CHECKVAL<std::string>(L, ++i);
        uint32 receiverGUIDLow = Eluna::CHECKVAL<uint32>(L, ++i);
        uint32 senderGUIDLow = Eluna::CHECKVAL<uint32>(L, ++i, 0);
        uint32 stationary = Eluna::CHECKVAL<uint32>(L, ++i, MAIL_STATIONERY_DEFAULT);
        uint32 delay = Eluna::CHECKVAL<uint32>(L, ++i, 0);
        int argAmount = lua_gettop(L);

        MailSender sender(MAIL_NORMAL, senderGUIDLow, (MailStationery)stationary);
        MailDraft draft(subject, text);

#ifndef MANGOS
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
#endif
        uint8 addedItems = 0;
        while (addedItems <= MAX_MAIL_ITEMS && i + 2 <= argAmount)
        {
            uint32 entry = luaL_checkunsigned(L, ++i);
            uint32 amount = luaL_checkunsigned(L, ++i);

#ifdef MANGOS
            ItemTemplate const* item_proto = ObjectMgr::GetItemPrototype(entry);
#else
            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(entry);
#endif
            if (!item_proto)
            {
                luaL_error(L, "Item entry %d does not exist", entry);
                continue;
            }
            if (amount < 1 || (item_proto->MaxCount > 0 && amount > uint32(item_proto->MaxCount)))
            {
                luaL_error(L, "Item entry %d has invalid amount %d", entry, amount);
                continue;
            }
            if (Item* item = Item::CreateItem(entry, amount))
            {
#ifdef MANGOS
                item->SaveToDB();
#else
                item->SaveToDB(trans);
                SQLTransaction trans = CharacterDatabase.BeginTransaction();
#endif
                draft.AddItem(item);
                ++addedItems;
            }
        }

#ifdef MANGOS
        draft.SendMailTo(MailReceiver(MAKE_NEW_GUID(receiverGUIDLow, 0, HIGHGUID_PLAYER)), sender);
#else
        draft.SendMailTo(trans, MailReceiver(receiverGUIDLow), sender, MAIL_CHECK_MASK_NONE, delay);
        CharacterDatabase.CommitTransaction(trans);
#endif
        return 0;
    }

    // bit_and(a, b)
    int bit_and(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 b = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, a & b);
        return 1;
    }

    // bit_or(a, b)
    int bit_or(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 b = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, a | b);
        return 1;
    }

    // bit_lshift(a, b)
    int bit_lshift(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 b = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, a << b);
        return 1;
    }

    // bit_rshift(a, b)
    int bit_rshift(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 b = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, a >> b);
        return 1;
    }

    // bit_xor(a, b)
    int bit_xor(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        uint32 b = Eluna::CHECKVAL<uint32>(L, 2);
        Eluna::Push(L, a ^ b);
        return 1;
    }

    // bit_not(a)
    int bit_not(lua_State* L)
    {
        uint32 a = Eluna::CHECKVAL<uint32>(L, 1);
        Eluna::Push(L, ~a);
        return 1;
    }

    // Helper
    uint32 AddPath(std::list<TaxiPathNodeEntry> nodes, uint32 mountA, uint32 mountH, uint32 price, uint32 pathId)
    {
        if (nodes.size() < 2)
            return 0;
        if (!pathId)
            pathId = sTaxiPathNodesByPath.size();
        if (sTaxiPathNodesByPath.size() <= pathId)
            sTaxiPathNodesByPath.resize(pathId + 1);
        sTaxiPathNodesByPath[pathId].clear();
        sTaxiPathNodesByPath[pathId].resize(nodes.size());
        static uint32 nodeId = 500;
        uint32 startNode = nodeId;
        uint32 index = 0;
        for (std::list<TaxiPathNodeEntry>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            TaxiPathNodeEntry entry = *it;
            entry.path = pathId;
            TaxiNodesEntry* nodeEntry = new TaxiNodesEntry();
            nodeEntry->ID = index;
            nodeEntry->map_id = entry.mapid;
            nodeEntry->MountCreatureID[0] = mountH;
            nodeEntry->MountCreatureID[1] = mountA;
            nodeEntry->x = entry.x;
            nodeEntry->y = entry.y;
            nodeEntry->z = entry.z;
            sTaxiNodesStore.SetEntry(nodeId, nodeEntry);
            entry.index = nodeId++;
            sTaxiPathNodesByPath[pathId].set(index++, TaxiPathNodePtr(new TaxiPathNodeEntry(entry)));
        }
        if (startNode >= nodeId)
            return 0;
        sTaxiPathSetBySource[startNode][nodeId - 1] = TaxiPathBySourceAndDestination(pathId, price);
        return pathId;
    }

    // AddTaxiPath(pathTable, mountA, mountH[, price, pathId])
    int AddTaxiPath(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TTABLE);
        uint32 mountA = Eluna::CHECKVAL<uint32>(L, 2);
        uint32 mountH = Eluna::CHECKVAL<uint32>(L, 3);
        uint32 price = Eluna::CHECKVAL<uint32>(L, 4, 0);
        uint32 pathId = Eluna::CHECKVAL<uint32>(L, 5, 0);
        lua_settop(L, 1);

        std::list<TaxiPathNodeEntry> nodes;

        int start = lua_gettop(L);
        int end = start;

        Eluna::Push(L);
        while (lua_next(L, -2) != 0)
        {
            luaL_checktype(L, -1, LUA_TTABLE);
            Eluna::Push(L);
            while (lua_next(L, -2) != 0)
            {
                lua_insert(L, end++);
            }
            if (start == end)
                continue;
            if (end - start < 4) // no mandatory args, dont add
            {
                while (end != start)
                    if (!lua_isnone(L, --end))
                        lua_remove(L, end);
                continue;
            }

            while (end - start < 8) // fill optional args with 0
            {
                Eluna::Push(L, 0);
                lua_insert(L, end++);
            }
            TaxiPathNodeEntry* entry = new TaxiPathNodeEntry();
            // mandatory
            entry->mapid = Eluna::CHECKVAL<uint32>(L, start);
            entry->x = Eluna::CHECKVAL<float>(L, start + 1);
            entry->y = Eluna::CHECKVAL<float>(L, start + 2);
            entry->z = Eluna::CHECKVAL<float>(L, start + 3);
            // optional
            entry->actionFlag = Eluna::CHECKVAL<uint32>(L, start + 4);
            entry->delay = Eluna::CHECKVAL<uint32>(L, start + 5);

            nodes.push_back(*entry);

            while (end != start) // remove args
                if (!lua_isnone(L, --end))
                    lua_remove(L, end);

            lua_pop(L, 1);
        }

        Eluna::Push(L, AddPath(nodes, mountA, mountH, price, pathId));
        return 1;
    }

    int AddCorpse(lua_State* L)
    {
        Corpse* corpse = Eluna::CHECKOBJ<Corpse>(L, 1);

        sObjectAccessor->AddCorpse(corpse);
        return 0;
    }

    int RemoveCorpse(lua_State* L)
    {
        Corpse* corpse = Eluna::CHECKOBJ<Corpse>(L, 1);
        sObjectAccessor->RemoveCorpse(corpse);
        return 1;
    }

    int ConvertCorpseForPlayer(lua_State* L)
    {
        uint64 guid = Eluna::CHECKVAL<uint64>(L, 1);
        bool insignia = Eluna::CHECKVAL<bool>(L, 2, false);

        Eluna::Push(L, sObjectAccessor->ConvertCorpseForPlayer(ObjectGuid(guid), insignia));
        return 0;
    }

    int RemoveOldCorpses(lua_State* L)
    {
        sObjectAccessor->RemoveOldCorpses();
        return 0;
    }

    int FindWeather(lua_State* L)
    {
        uint32 zoneId = Eluna::CHECKVAL<uint32>(L, 1);
#ifdef MANGOS
        Weather* weather = sWorld->FindWeather(zoneId);
#else
        Weather* weather = WeatherMgr::FindWeather(zoneId);
#endif
        Eluna::Push(L, weather);
        return 1;
    }

    int AddWeather(lua_State* L)
    {
        uint32 zoneId = Eluna::CHECKVAL<uint32>(L, 1);
#ifdef MANGOS
        Weather* weather = sWorld->AddWeather(zoneId);
#else
        Weather* weather = WeatherMgr::AddWeather(zoneId);
#endif
        Eluna::Push(L, weather);
        return 1;
    }

    int RemoveWeather(lua_State* L)
    {
        uint32 zoneId = Eluna::CHECKVAL<uint32>(L, 1);
#ifdef MANGOS
        sWorld->RemoveWeather(zoneId);
#else
        WeatherMgr::RemoveWeather(zoneId);
#endif
        return 0;
    }

    int SendFineWeatherToPlayer(lua_State* L)
    {
        Player* player = Eluna::CHECKOBJ<Player>(L, 1);
#ifdef MANGOS
        Weather::SendFineWeatherUpdateToPlayer(player);
#else
        WeatherMgr::SendFineWeatherUpdateToPlayer(player);
#endif
        return 0;
    }

    int DoForPlayersInWorld(lua_State* L)
    {
        luaL_checktype(L, 1, LUA_TFUNCTION);
        uint32 team = Eluna::CHECKVAL<uint32>(L, 2, TEAM_NEUTRAL);
        bool onlyGM = Eluna::CHECKVAL<bool>(L, 3, false);

        Eluna* E = Eluna::GetEluna(L);
        if (!E)
            return 0;

        TRINITY_READ_GUARD(HashMapHolder<Player>::LockType, *HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType const& m = sObjectAccessor->GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            Player* player = itr->second;

            if (team != TEAM_NEUTRAL && player->GetTeamId() == team)
                continue;
#ifdef MANGOS
            if (onlyGM && player->isGameMaster())
                continue;
#else
            if (onlyGM && player->IsGameMaster())
                continue;
#endif
            lua_pushvalue(L, 1); // function
            Eluna::Push(L, player);
            E->ExecuteCall(1, 0);
        }
        return 0;
    }

    int map_SendStateMsg(lua_State* L)
    {
        int i = 0;
        uint32 target_map = Eluna::CHECKVAL<uint32>(L, ++i, MAPID_INVALID);
        uint32 target_instanceid = Eluna::CHECKVAL<uint32>(L, ++i, 0);
        StateMsg* msg = new StateMsg(target_map, target_instanceid);
        while (!lua_isnone(L, ++i))
        {
            if (lua_isnil(L, i))
                msg->AddValue();
            else if (lua_isboolean(L, i))
                msg->AddValue(Eluna::CHECKVAL<bool>(L, i));
            else if (lua_isstring(L, i))
                msg->AddValue(Eluna::CHECKVAL<std::string>(L, i));
            else if (lua_isnumber(L, i))
                msg->AddValue(Eluna::CHECKVAL<double>(L, i));
            else
                luaL_argerror(L, i, "Unsupported argument type");
        }
        Eluna::AddStateMsg(msg);
        return 0;
    }

    int world_SendStateMsg(lua_State* L)
    {
        int i = 0;
        uint32 target_map = Eluna::CHECKVAL<uint32>(L, ++i, MAPID_INVALID);
        uint32 target_instanceid = Eluna::CHECKVAL<uint32>(L, ++i, 0);
        StateMsg* msg = new StateMsg(target_map, target_instanceid);
        while (!lua_isnone(L, ++i))
        {
            if (lua_isnil(L, i))
                msg->AddValue();
            else if (lua_isboolean(L, i))
                msg->AddValue(Eluna::CHECKVAL<bool>(L, i));
            else if (lua_isstring(L, i))
                msg->AddValue(Eluna::CHECKVAL<std::string>(L, i));
            else if (lua_isnumber(L, i))
                msg->AddValue(Eluna::CHECKVAL<double>(L, i));
            else
                luaL_argerror(L, i, "Unsupported argument type");
        }
        // Send directly as this thread safe for world
        if (Eluna* E = msg->GetTarget())
            E->OnStateMessage(msg);
        delete msg;
        return 0;
    }
}
#endif
