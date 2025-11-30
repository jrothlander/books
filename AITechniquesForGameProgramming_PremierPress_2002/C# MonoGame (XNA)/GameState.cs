namespace Section_AI_1_1_Pathfinder;

public static class GameState
{
    public static GameStates State { get; set; } = GameStates.Title;

    public enum GameStates
    {
        Title,
        Start,
        Play,
        Pause,
        Stop
    };

    public static void Stop()
    {
        State = GameStates.Stop;
    }

    public static void Reset()
    {
        State = GameStates.Title;
    }
}