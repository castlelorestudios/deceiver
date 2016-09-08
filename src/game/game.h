#pragma once
#include "data/array.h"
#include "ai.h"
#include "input.h"
#include "render/render.h"
#include "render/views.h"
#include <unordered_map>

namespace VI
{

struct RenderParams;

typedef void(*UpdateFunction)(const Update&);
typedef void(*DrawFunction)(const RenderParams&);
typedef void(*CleanupFunction)();

struct EntityFinder
{
	struct NameEntry
	{
		const char* name;
		Ref<Entity> entity;
		cJSON* properties;
	};

	Array<NameEntry> map;
	Entity* find(const char*) const;
};

#if DEBUG
#define DEBUG_AI_CONTROL 0
#endif

struct Game
{
	enum class Mode
	{
		Special,
		Pvp,
	};

	enum class FeatureLevel
	{
		Base,
		HealthPickups,
		Abilities,
		All,
		count = All,
	};

	enum class NetworkQuality
	{
		Perfect,
		Okay,
		Bad,
	};

	enum class NetworkState
	{
		Normal,
		Lag,
		Recover,
	};

	enum class MatchResult
	{
		None,
		Victory,
		Loss,
		NetworkError,
		OpponentQuit,
		Draw,
	};

	struct Session
	{
		AI::Team local_player_config[MAX_GAMEPADS];
		Mode mode;
		NetworkQuality network_quality;
		NetworkState network_state;
		MatchResult last_match;
		r32 network_time;
		r32 network_timer;
		r32 time_scale;
		AI::Team teams;
		AssetID level;
		b8 third_person;
		b8 local_multiplayer;

		Session();
		void reset();
		r32 effective_time_scale() const;
		s32 local_player_count() const;
	};

	enum class ZoneState
	{
		Inaccessible,
		Locked,
		Friendly,
		Hostile,
		Owned,
	};

	enum class Resource
	{
		Energy,
		HackKits,
		count,
	};

	struct Message
	{
		r64 timestamp;
		AssetID contact;
		AssetID text;
		b8 read;
	};

	struct Save
	{
		Array<Message> messages;
		Array<Message> messages_scheduled;
		std::unordered_map<AssetID, AssetID> variables; // todo: kill STL
		ZoneState zones[64];
		u16 story_index;
		u16 resources[(s32)Resource::count];
		const char* username;

		Save();
	};

	struct Level
	{
		FeatureLevel feature_level;
		r32 min_y;
		Skybox::Config skybox;
		b8 lock_teams;
		b8 continue_match_after_death;

		b8 has_feature(FeatureLevel) const;
	};

	static Session session;
	static Save save;
	static Level level;

	static b8 quit;
	static GameTime time;
	static GameTime real_time;
	static r32 physics_timestep;
	static AssetID scheduled_load_level;
	static Mode scheduled_mode;
	static Array<UpdateFunction> updates;
	static Array<DrawFunction> draws;
	static Array<CleanupFunction> cleanups;
	static b8 cancel_event_eaten[MAX_GAMEPADS];

	static b8 init(LoopSync*);
	static void execute(const Update&, const char*);
	static void update(const Update&);
	static void schedule_load_level(AssetID, Mode);
	static void unload_level();
	static void load_level(const Update&, AssetID, Mode, b8 = false);
	static void draw_opaque(const RenderParams&);
	static void draw_alpha(const RenderParams&);
	static void draw_additive(const RenderParams&);
	static void draw_override(const RenderParams&);
	static void term();
};

}
