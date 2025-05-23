/* -----------------------------------------------------------------------
 * GGPO.net (http://ggpo.net)  -  Copyright 2009 GroundStorm Studios, LLC.
 *
 * Use of this software is governed by the MIT license that can be found
 * in the LICENSE file.
 */

#pragma once

#include "CoreMinimal.h"
#include <stdarg.h>
#include "../../Private/log.h"
#include "ggponet.generated.h"

UENUM(BlueprintType)
enum class EGGPOPlayerType : uint8
{
    LOCAL      UMETA(DisplayName = "Local"),
    REMOTE     UMETA(DisplayName = "Remote"),
    SPECTATOR  UMETA(DisplayName = "Spectator"),
};
/*
 * The GGPONetworkStats function contains some statistics about the current
 * session.
 *
 * network.send_queue_len - The length of the queue containing UDP packets
 * which have not yet been acknowledged by the end client.  The length of
 * the send queue is a rough indication of the quality of the connection.
 * The longer the send queue, the higher the round-trip time between the
 * clients.  The send queue will also be longer than usual during high
 * packet loss situations.
 *
 * network.recv_queue_len - The number of inputs currently buffered by the
 * GGPO.net network layer which have yet to be validated.  The length of
 * the prediction queue is roughly equal to the current frame number
 * minus the frame number of the last packet in the remote queue.
 *
 * network.ping - The roundtrip packet transmission time as calcuated
 * by GGPO.net.  This will be roughly equal to the actual round trip
 * packet transmission time + 2 the interval at which you call ggpo_idle
 * or ggpo_advance_frame.
 *
 * network.kbps_sent - The estimated bandwidth used between the two
 * clients, in kilobits per second.
 *
 * timesync.local_frames_behind - The number of frames GGPO.net calculates
 * that the local client is behind the remote client at this instant in
 * time.  For example, if at this instant the current game client is running
 * frame 1002 and the remote game client is running frame 1009, this value
 * will mostly likely roughly equal 7.
 *
 * timesync.remote_frames_behind - The same as local_frames_behind, but
 * calculated from the perspective of the remote player.
 *
 */
USTRUCT(BlueprintType)
struct FGGPONetworkInfo {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   send_queue_len;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   recv_queue_len;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   ping;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   kbps_sent;
};

USTRUCT(BlueprintType)
struct FGGPOSyncInfo {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   local_frames_behind;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32   remote_frames_behind;
};

USTRUCT(BlueprintType)
struct FGGPONetworkStats {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FGGPONetworkInfo network;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FGGPOSyncInfo timesync;
};

/**
 * A network address object.
 * Composed of an ip address and a port.
 */
UCLASS(Blueprintable)
class GGPOUE_API UGGPONetworkAddress : public UObject
{
    GENERATED_BODY()

private:
    UPROPERTY()
        bool bValidAddress = true;
    char IpAddress[32];
    uint16 Port = 0;

public:
    UGGPONetworkAddress() {}

    /** Creates a GGPO network address. */
    UFUNCTION(BlueprintCallable, Category = "GGPO")
        static UGGPONetworkAddress* CreateNetworkAddress(UObject* Outer, const FName Name, const FString Address);
    /** Creates a GGPO local address. */
    UFUNCTION(BlueprintCallable, Category = "GGPO")
        static UGGPONetworkAddress* CreateLocalAddress(UObject* Outer, const FName Name, int32 LocalPort);

    /** Outputs the ip address array to the passed pointer. */
    void GetIpAddress(char OutAddress[32]) const;

    /** Returns whether the address is valid. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        bool IsValidAddress() const;
    /** Gets the ip address as a string. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        FString GetIpAddressString() const;
    /** Gets the port. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 GetPort() const;

    /** Gets the port. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        bool IsSameAddress(const UGGPONetworkAddress* Other) const;

};

/**
 * A collection of network addresses.
 */
UCLASS(Blueprintable)
class GGPOUE_API UGGPONetwork : public UObject
{
    GENERATED_BODY()

private:
    UPROPERTY()
        int32 LocalPlayerIndex = -1;
    UPROPERTY()
        int32 NumPlayers = 0;
    UPROPERTY()
        int32 LocalPort = 7000;
    UPROPERTY()
        TArray<UGGPONetworkAddress*> Addresses;
    UPROPERTY()
        TArray<UGGPONetworkAddress*> Spectators;

public:
    UGGPONetwork() {}

    /** Creates a collection of network addresses. */
    UFUNCTION(BlueprintCallable, Category = "GGPO")
        static UGGPONetwork* CreateNetwork(UObject* Outer, const FName Name, int32 InNumPlayers, int32 PlayerIndex, int32 InLocalPort, TArray<FString> RemoteAddresses);

    /** Returns true if all addresses are valid. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        bool AllValidAddresses() const;
    /** Returns true if all addresses are unique. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        bool AllUniqueAddresses() const;

    /** Gets one network address. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        UGGPONetworkAddress* GetAddress(int32 Index) const;
    /** Gets the total number of players on the network. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 NumAddresses() const;
    /** Gets the local player index. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 GetLocalPlayerIndex() const { return LocalPlayerIndex; }

    /** Gets one spectator. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        UGGPONetworkAddress* GetSpectator(int32 Index) const;
    /** Gets the total number of players on the network. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 NumSpectators() const;

    /** Gets number of players in the game session. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 GetNumPlayers() const { return NumPlayers; }

    /** Gets the local port. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        int32 GetLocalPort() const;

    /** Returns true if this game instance is spectating. */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GGPO")
        bool IsSpectator() const
    {
        return LocalPlayerIndex < 0;
    }

};


#ifdef __cplusplus
extern "C" {
#endif


// On windows, export at build time and import at runtime.
// ELF systems don't need an explicit export/import.
#ifdef _WIN32
#  if defined(GGPO_SHARED_LIB)
#  ifdef GGPO_SDK_EXPORT
#       define GGPO_API __declspec(dllexport)
#     else
#       define GGPO_API __declspec(dllimport)
#     endif
#  else
#     define GGPO_API
#  endif
#else
#  define GGPO_API
#endif

#define GGPO_MAX_PLAYERS                  4
#define GGPO_MAX_PREDICTION_FRAMES        8
#define GGPO_MAX_SPECTATORS              32

#define GGPO_SPECTATOR_INPUT_INTERVAL     4

typedef struct GGPOSession GGPOSession;

typedef int32 GGPOPlayerHandle;

/*
 * The GGPOPlayer structure used to describe players in ggpo_add_player
 *
 * size: Should be set to the sizeof(GGPOPlayer)
 *
 * type: One of the EGGPOPlayerType values describing how inputs should be handled
 *       Local players must have their inputs updated every frame via
 *       ggpo_add_local_inputs.  Remote players values will come over the
 *       network.
 *
 * player_num: The player number.  Should be between 1 and the number of players
 *       In the game (e.g. in a 2 player game, either 1 or 2).
 *
 * If type == EGGPOPlayerType::REMOTE:
 * 
 * u.remote.ip_address:  The ip address of the ggpo session which will host this
 *       player.
 *
 * u.remote.port: The port where udp packets should be sent to reach this player.
 *       All the local inputs for this session will be sent to this player at
 *       ip_address:port.
 *
 */

typedef struct GGPOPlayer {
   int               size;
   EGGPOPlayerType    type;
   int               player_num;
   union {
      struct {
      } local;
      struct {
         char           ip_address[32];
         unsigned short port;
      } remote;
   } u;
} GGPOPlayer;

typedef struct GGPOLocalEndpoint {
   int      player_num;
} GGPOLocalEndpoint;


#define GGPO_ERRORLIST                                               \
   GGPO_ERRORLIST_ENTRY(GGPO_OK,                               0)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_SUCCESS,                0)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_GENERAL_FAILURE,        -1)   \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_INVALID_SESSION,        1)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_INVALID_PLAYER_HANDLE,  2)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_PLAYER_OUT_OF_RANGE,    3)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_PREDICTION_THRESHOLD,   4)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_UNSUPPORTED,            5)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_NOT_SYNCHRONIZED,       6)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_IN_ROLLBACK,            7)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_INPUT_DROPPED,          8)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_PLAYER_DISCONNECTED,    9)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_TOO_MANY_SPECTATORS,   10)    \
   GGPO_ERRORLIST_ENTRY(GGPO_ERRORCODE_INVALID_REQUEST,       11)

#define GGPO_ERRORLIST_ENTRY(name, value)       name = value,
typedef enum {
   GGPO_ERRORLIST
} GGPOErrorCode;
#undef GGPO_ERRORLIST_ENTRY

#define GGPO_SUCCEEDED(result)      ((result) == GGPO_ERRORCODE_SUCCESS)


#define GGPO_INVALID_HANDLE      (-1)


/*
 * The GGPOEventCode enumeration describes what type of event just happened.
 *
 * GGPO_EVENTCODE_CONNECTED_TO_PEER - Handshake with the game running on the
 * other side of the network has been completed.
 * 
 * GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER - Beginning the synchronization
 * process with the client on the other end of the networking.  The count
 * and total fields in the u.synchronizing struct of the GGPOEvent
 * object indicate progress.
 *
 * GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER - The synchronziation with this
 * peer has finished.
 *
 * GGPO_EVENTCODE_RUNNING - All the clients have synchronized.  You may begin
 * sending inputs with ggpo_synchronize_inputs.
 *
 * GGPO_EVENTCODE_DISCONNECTED_FROM_PEER - The network connection on 
 * the other end of the network has closed.
 *
 * GGPO_EVENTCODE_TIMESYNC - The time synchronziation code has determined
 * that this client is too far ahead of the other one and should slow
 * down to ensure fairness.  The u.timesync.frames_ahead parameter in
 * the GGPOEvent object indicates how many frames the client is.
 *
 */
typedef enum {
   GGPO_EVENTCODE_CONNECTED_TO_PEER            = 1000,
   GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER      = 1001,
   GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER       = 1002,
   GGPO_EVENTCODE_RUNNING                      = 1003,
   GGPO_EVENTCODE_DISCONNECTED_FROM_PEER       = 1004,
   GGPO_EVENTCODE_TIMESYNC                     = 1005,
   GGPO_EVENTCODE_CONNECTION_INTERRUPTED       = 1006,
   GGPO_EVENTCODE_CONNECTION_RESUMED           = 1007,
} GGPOEventCode;

/*
 * The GGPOEvent structure contains an asynchronous event notification sent
 * by the on_event callback.  See GGPOEventCode, above, for a detailed
 * explanation of each event.
 */
typedef struct {
   GGPOEventCode code;
   union {
      struct {
         GGPOPlayerHandle  player;
      } connected;
      struct {
         GGPOPlayerHandle  player;
         int               count;
         int               total;
      } synchronizing;
      struct {
         GGPOPlayerHandle  player;
      } synchronized;
      struct {
         GGPOPlayerHandle  player;
      } disconnected;
      struct {
         int               frames_ahead;
      } timesync;
      struct {
         GGPOPlayerHandle  player;
         int               disconnect_timeout;
      } connection_interrupted;
      struct {
         GGPOPlayerHandle  player;
      } connection_resumed;
   } u;
} GGPOEvent;


// If C++11, use std::function instead of function pointers for GGPOSessionCallbacks
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)

// Close extern "C"
};

/*
 * The GGPOSessionCallbacks structure contains the callback functions that
 * your application must implement.  GGPO.net will periodically call these
 * functions during the game.  All callback functions must be implemented.
 */
struct GGPOSessionCallbacks {
    /*
     * begin_game callback - This callback has been deprecated.  You must
     * implement it, but should ignore the 'game' parameter.
     */
    std::function<bool(const char* game)> begin_game;

    /*
     * save_game_state - The client should allocate a buffer, copy the
     * entire contents of the current game state into it, and copy the
     * length into the *len parameter.  Optionally, the client can compute
     * a checksum of the data and store it in the *checksum argument.
     */
    std::function<bool(unsigned char** buffer, int* len, int* checksum, int frame)> save_game_state;

    /*
     * load_game_state - GGPO.net will call this function at the beginning
     * of a rollback.  The buffer and len parameters contain a previously
     * saved state returned from the save_game_state function.  The client
     * should make the current game state match the state contained in the
     * buffer.
     */
    std::function<bool(unsigned char* buffer, int len)> load_game_state;

    /*
     * log_game_state - Used in diagnostic testing.  The client should use
     * the ggpo_log function to write the contents of the specified save
     * state in a human readible form.
     */
    std::function<bool(char* filename, unsigned char* buffer, int len)> log_game_state;

    /*
     * free_buffer - Frees a game state allocated in save_game_state.  You
     * should deallocate the memory contained in the buffer.
     */
    std::function<void(void* buffer)> free_buffer;

    /*
     * advance_frame - Called during a rollback.  You should advance your game
     * state by exactly one frame.  Before each frame, call ggpo_synchronize_input
     * to retrieve the inputs you should use for that frame.  After each frame,
     * you should call ggpo_advance_frame to notify GGPO.net that you're
     * finished.
     *
     * The flags parameter is reserved.  It can safely be ignored at this time.
     */
    std::function<bool(int flags)> advance_frame;

    /*
     * on_event - Notification that something has happened.  See the GGPOEventCode
     * structure above for more information.
     */
    std::function<bool(GGPOEvent * info)> on_event;
};

extern "C" {
#else

/*
 * The GGPOSessionCallbacks structure contains the callback functions that
 * your application must implement.  GGPO.net will periodically call these
 * functions during the game.  All callback functions must be implemented.
 */
 typedef struct {
     /*
      * begin_game callback - This callback has been deprecated.  You must
      * implement it, but should ignore the 'game' parameter.
      */
     bool(__cdecl* begin_game)(const char* game);

     /*
      * save_game_state - The client should allocate a buffer, copy the
      * entire contents of the current game state into it, and copy the
      * length into the *len parameter.  Optionally, the client can compute
      * a checksum of the data and store it in the *checksum argument.
      */
     bool(__cdecl* save_game_state)(unsigned char** buffer, int* len, int* checksum, int frame);

     /*
      * load_game_state - GGPO.net will call this function at the beginning
      * of a rollback.  The buffer and len parameters contain a previously
      * saved state returned from the save_game_state function.  The client
      * should make the current game state match the state contained in the
      * buffer.
      */
     bool(__cdecl* load_game_state)(unsigned char* buffer, int len);

     /*
      * log_game_state - Used in diagnostic testing.  The client should use
      * the ggpo_log function to write the contents of the specified save
      * state in a human readible form.
      */
     bool(__cdecl* log_game_state)(char* filename, unsigned char* buffer, int len);

     /*
      * free_buffer - Frees a game state allocated in save_game_state.  You
      * should deallocate the memory contained in the buffer.
      */
     void(__cdecl* free_buffer)(void* buffer);

     /*
      * advance_frame - Called during a rollback.  You should advance your game
      * state by exactly one frame.  Before each frame, call ggpo_synchronize_input
      * to retrieve the inputs you should use for that frame.  After each frame,
      * you should call ggpo_advance_frame to notify GGPO.net that you're
      * finished.
      *
      * The flags parameter is reserved.  It can safely be ignored at this time.
      */
     bool(__cdecl* advance_frame)(int flags);

     /*
      * on_event - Notification that something has happened.  See the GGPOEventCode
      * structure above for more information.
      */
     bool(__cdecl* on_event)(GGPOEvent* info);
 } GGPOSessionCallbacks;

#endif


class GGPOUE_API GGPONet
{
public:
    /*
     * ggpo_start_session --
     *
     * Used to being a new GGPO.net session.  The ggpo object returned by ggpo_start_session
     * uniquely identifies the state for this session and should be passed to all other
     * functions.
     *
     * session - An out parameter to the new ggpo session object.
     *
     * cb - A GGPOSessionCallbacks structure which contains the callbacks you implement
     * to help GGPO.net synchronize the two games.  You must implement all functions in
     * cb, even if they do nothing but 'return true';
     *
     * game - The name of the game.  This is used internally for GGPO for logging purposes only.
     *
     * num_players - The number of players which will be in this game.  The number of players
     * per session is fixed.  If you need to change the number of players or any player
     * disconnects, you must start a new session.
     *
     * input_size - The size of the game inputs which will be passsed to ggpo_add_local_input.
     *
     * local_port - The port GGPO should bind to for UDP traffic.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_start_session(GGPOSession** session,
        GGPOSessionCallbacks* cb,
        const char* game,
        int num_players,
        int input_size,
        unsigned short localport);


    /*
     * ggpo_add_player --
     *
     * Must be called for each player in the session (e.g. in a 3 player session, must
     * be called 3 times).
     *
     * player - A GGPOPlayer struct used to describe the player.
     *
     * handle - An out parameter to a handle used to identify this player in the future.
     * (e.g. in the on_event callbacks).
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_add_player(GGPOSession* session,
        GGPOPlayer* player,
        GGPOPlayerHandle* handle);


    /*
     * ggpo_start_synctest --
     *
     * Used to being a new GGPO.net sync test session.  During a sync test, every
     * frame of execution is run twice: once in prediction mode and once again to
     * verify the result of the prediction.  If the checksums of your save states
     * do not match, the test is aborted.
     *
     * cb - A GGPOSessionCallbacks structure which contains the callbacks you implement
     * to help GGPO.net synchronize the two games.  You must implement all functions in
     * cb, even if they do nothing but 'return true';
     *
     * game - The name of the game.  This is used internally for GGPO for logging purposes only.
     *
     * num_players - The number of players which will be in this game.  The number of players
     * per session is fixed.  If you need to change the number of players or any player
     * disconnects, you must start a new session.
     *
     * input_size - The size of the game inputs which will be passsed to ggpo_add_local_input.
     *
     * frames - The number of frames to run before verifying the prediction.  The
     * recommended value is 1.
     *
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_start_synctest(GGPOSession** session,
        GGPOSessionCallbacks* cb,
        char* game,
        int num_players,
        int input_size,
        int frames);


    /*
     * ggpo_start_spectating --
     *
     * Start a spectator session.
     *
     * cb - A GGPOSessionCallbacks structure which contains the callbacks you implement
     * to help GGPO.net synchronize the two games.  You must implement all functions in
     * cb, even if they do nothing but 'return true';
     *
     * game - The name of the game.  This is used internally for GGPO for logging purposes only.
     *
     * num_players - The number of players which will be in this game.  The number of players
     * per session is fixed.  If you need to change the number of players or any player
     * disconnects, you must start a new session.
     *
     * input_size - The size of the game inputs which will be passsed to ggpo_add_local_input.
     *
     * local_port - The port GGPO should bind to for UDP traffic.
     *
     * host_ip - The IP address of the host who will serve you the inputs for the game.  Any
     * player partcipating in the session can serve as a host.
     *
     * host_port - The port of the session on the host
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_start_spectating(GGPOSession** session,
        GGPOSessionCallbacks* cb,
        const char* game,
        int num_players,
        int input_size,
        unsigned short local_port,
        char* host_ip,
        unsigned short host_port);

    /*
     * ggpo_close_session --
     * Used to close a session.  You must call ggpo_close_session to
     * free the resources allocated in ggpo_start_session.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_close_session(GGPOSession*);


    /*
     * ggpo_set_frame_delay --
     *
     * Change the amount of frames ggpo will delay local input.  Must be called
     * before the first call to ggpo_synchronize_input.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_set_frame_delay(GGPOSession*,
        GGPOPlayerHandle player,
        int frame_delay);

    /*
     * ggpo_idle --
     * Should be called periodically by your application to give GGPO.net
     * a chance to do some work.  Most packet transmissions and rollbacks occur
     * in ggpo_idle.
     *
     * timeout - The amount of time GGPO.net is allowed to spend in this function,
     * in milliseconds.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_idle(GGPOSession*,
        int timeout);

    /*
     * ggpo_add_local_input --
     *
     * Used to notify GGPO.net of inputs that should be trasmitted to remote
     * players.  ggpo_add_local_input must be called once every frame for
     * all player of type EGGPOPlayerType::LOCAL.
     *
     * player - The player handle returned for this player when you called
     * ggpo_add_local_player.
     *
     * values - The controller inputs for this player.
     *
     * size - The size of the controller inputs.  This must be exactly equal to the
     * size passed into ggpo_start_session.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_add_local_input(GGPOSession*,
        GGPOPlayerHandle player,
        void* values,
        int size);

    /*
     * ggpo_synchronize_input --
     *
     * You should call ggpo_synchronize_input before every frame of execution,
     * including those frames which happen during rollback.
     *
     * values - When the function returns, the values parameter will contain
     * inputs for this frame for all players.  The values array must be at
     * least (size * players) large.
     *
     * size - The size of the values array.
     *
     * disconnect_flags - Indicated whether the input in slot (1 << flag) is
     * valid.  If a player has disconnected, the input in the values array for
     * that player will be zeroed and the i-th flag will be set.  For example,
     * if only player 3 has disconnected, disconnect flags will be 8 (i.e. 1 << 3).
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_synchronize_input(GGPOSession*,
        void* values,
        int size,
        int* disconnect_flags);

    /*
     * ggpo_disconnect_player --
     *
     * Disconnects a remote player from a game.  Will return GGPO_ERRORCODE_PLAYER_DISCONNECTED
     * if you try to disconnect a player who has already been disconnected.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_disconnect_player(GGPOSession*,
        GGPOPlayerHandle player);

    /*
     * ggpo_advance_frame --
     *
     * You should call ggpo_advance_frame to notify GGPO.net that you have
     * advanced your gamestate by a single frame.  You should call this everytime
     * you advance the gamestate by a frame, even during rollbacks.  GGPO.net
     * may call your save_state callback before this function returns.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_advance_frame(GGPOSession*);

    /*
     * ggpo_get_network_stats --
     *
     * Used to fetch some statistics about the quality of the network connection.
     *
     * player - The player handle returned from the ggpo_add_player function you used
     * to add the remote player.
     *
     * stats - Out parameter to the network statistics.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_get_network_stats(GGPOSession*,
        GGPOPlayerHandle player,
        FGGPONetworkStats* stats);


    /*
     * ggpo_set_disconnect_timeout --
     *
     * Sets the disconnect timeout.  The session will automatically disconnect
     * from a remote peer if it has not received a packet in the timeout window.
     * You will be notified of the disconnect via a GGPO_EVENTCODE_DISCONNECTED_FROM_PEER
     * event.
     *
     * Setting a timeout value of 0 will disable automatic disconnects.
     *
     * timeout - The time in milliseconds to wait before disconnecting a peer.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_set_disconnect_timeout(GGPOSession*,
        int timeout);

    /*
     * ggpo_set_disconnect_notify_start --
     *
     * The time to wait before the first GGPO_EVENTCODE_NETWORK_INTERRUPTED timeout
     * will be sent.
     *
     * timeout - The amount of time which needs to elapse without receiving a packet
     *           before the GGPO_EVENTCODE_NETWORK_INTERRUPTED event is sent.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_set_disconnect_notify_start(GGPOSession*,
        int timeout);

    /*
     * ggpo_try_synchronize_local --
     *
     * Attempts to skip waiting for a synchronize message, if the session is single
     * player with no spectators ie. completely local.
     */
    static GGPO_API GGPOErrorCode __cdecl ggpo_try_synchronize_local(GGPOSession* ggpo);


    /*
     * ggpo_log --
     *
     * Used to write to the Unreal Engine log.
     * The log is controlled from the project settings using GGPOUE_Settings.
     */
    static GGPO_API void __cdecl ggpo_log(GGPOSession*,
        const char* fmt, ...);
    static GGPO_API void __cdecl ggpo_log(GGPOSession*,
        EGGPOLogVerbosity Verbosity,
        const char* fmt, ...);
    /*
     * ggpo_logv --
     *
     * A varargs compatible version of ggpo_log.  See ggpo_log for
     * more details.
     */
    static GGPO_API void __cdecl ggpo_logv(GGPOSession*,
        EGGPOLogVerbosity Verbosity,
        const char* fmt,
        va_list args);
};

#ifdef __cplusplus
};
#endif
