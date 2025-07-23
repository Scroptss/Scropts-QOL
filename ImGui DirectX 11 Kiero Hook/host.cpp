#include "host.h"

namespace host {

	void setforcehostdvars(bool ShouldSet) {

		if (ShouldSet) {
			Dvar_SetFromString("party_minPlayers", "1", true);
			Dvar_SetFromString("lobbyDedicatedSearchSkip", "1", true);
			Dvar_SetFromString("lobbySearchTeamSize", "1", true);
			Dvar_SetFromString("lobbySearchSkip", "1", true);
			Dvar_SetFromString("lobbyMergeDedicatedEnabled", "0", true);
			Dvar_SetFromString("lobbySearchListenCountries", "0,103,6,5,8,13,16,23,25,32,34,24,37,42,44,50,71,74,76,75,82,84,88,31,90,18,35", true);
			Dvar_SetFromString("excellentPing", "3", true);
			Dvar_SetFromString("goodPing", "4", true);
			Dvar_SetFromString("terriblePing", "5", true);
			Dvar_SetFromString("migration_forceHost", "1", true);
			Dvar_SetFromString("migration_minclientcount", "12", true);
			Dvar_SetFromString("party_connectToOthers", "0", true);
			Dvar_SetFromString("party_dedicatedOnly", "0", true);
			Dvar_SetFromString("party_dedicatedMergeMinPlayers", "12", true);
			Dvar_SetFromString("party_forceMigrateAfterRound", "0", true);
			Dvar_SetFromString("party_forceMigrateOnMatchStartRegression", "0", true);
			Dvar_SetFromString("party_joinInProgressAllowed", "1", true);
			Dvar_SetFromString("allowAllNAT", "1", true);
			Dvar_SetFromString("party_keepPartyAliveWhileMatchmaking", "1", true);
			Dvar_SetFromString("party_mergingEnabled", "0", true);
			Dvar_SetFromString("party_neverJoinRecent", "1", true);
			Dvar_SetFromString("party_readyPercentRequired", ".25", true);
			Dvar_SetFromString("partyMigrate_disabled", "1", true);
		}

		else {

			Dvar_SetFromString("party_minPlayers", "6", true);
			Dvar_SetFromString("lobbyDedicatedSearchSkip", "0", true);
			Dvar_SetFromString("lobbySearchTeamSize", "0", true);
			Dvar_SetFromString("lobbySearchSkip", "0", true);
			Dvar_SetFromString("lobbyMergeDedicatedEnabled", "1", true);
			Dvar_SetFromString("lobbySearchListenCountries", "0", true);
			Dvar_SetFromString("excellentPing", "30", true);
			Dvar_SetFromString("goodPing", "100", true);
			Dvar_SetFromString("terriblePing", "500", true);
			Dvar_SetFromString("migration_forceHost", "0", true);
			Dvar_SetFromString("migration_minclientcount", "2", true);
			Dvar_SetFromString("party_connectToOthers", "1", true);
			Dvar_SetFromString("party_dedicatedOnly", "0", true);
			Dvar_SetFromString("party_dedicatedMergeMinPlayers", "2", true);
			Dvar_SetFromString("party_forceMigrateAfterRound", "0", true);
			Dvar_SetFromString("party_forceMigrateOnMatchStartRegression", "0", true);
			Dvar_SetFromString("party_joinInProgressAllowed", "1", true);
			Dvar_SetFromString("allowAllNAT", "1", true);
			Dvar_SetFromString("party_keepPartyAliveWhileMatchmaking", "1", true);
			Dvar_SetFromString("party_mergingEnabled", "1", true);
			Dvar_SetFromString("party_neverJoinRecent", "0", true);
			Dvar_SetFromString("party_readyPercentRequired", "1.0", true);
			Dvar_SetFromString("partyMigrate_disabled", "0", true);

		}

	}

}