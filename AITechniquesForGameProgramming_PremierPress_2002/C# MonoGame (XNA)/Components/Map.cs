using System;
using System.Collections.Generic;
using FontStashSharp;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.IO;
//using Microsoft.Extensions.DependencyInjection;

namespace Section_AI_1_1_Pathfinder.Components;

public class Tile
{
    public enum Types { Closed, Open };
    public int X { get; set; }
    public int Y { get; set; }
    public Color Color { get; set; }
    public Types Type { get; set; }
}

public static class Configuration
{
    public static Tile[,] Tiles { get; set; }
    public static int Generation = 0;
}

internal class Map : DrawableGameComponent
{
    private readonly int[,] _map = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
        { 8, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
        { 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
        { 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1 },
        { 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1 },
        { 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 5 },
        { 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
    };

    //private readonly int[,] _map = {
    //    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    //    { 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 },
    //    { 8, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1 },
    //    { 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
    //    { 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    //    { 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
    //    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1 },
    //    { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5 },
    //    { 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    //    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
    //};

    //private readonly int[,] _map = {
    //    { 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    //    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5 }
    //};

    private const int MapHeight = 10;
    private const int MapWidth = 15;
    private const int ScaleWidth = 26;
    private const int ScaleHeight = 22;

    const int m_iMapHeight = MapHeight;
    const int m_iMapWidth = MapWidth;

    // Position of start rectangle
    const int m_iStartX = 14;
    const int m_iStartY = 7; //9;

    // Position of end rectangle
    const int m_iEndX = 0;
    const int m_iEndY = 2;
    //const int m_iEndX = 0;
    //const int m_iEndY = 1;

    private readonly int[,] _memory = new int[MapHeight, MapWidth];

    private new Game Game { get; }
    private SpriteBatch _spriteBatch;
    private Texture2D _texture;
    private FontSystem _fontSystem;

    public Map(Game game) : base(game)
    {
        Game = game;
    }


    //private readonly SpriteBatch _spriteBatch;

    //public ScreenManagementService(SpriteBatch spriteBatch)
    //{
    //    _spriteBatch = spriteBatch;
    //}


    public override void Initialize()
    {
        _spriteBatch = (SpriteBatch)Game.Services.GetService(typeof(SpriteBatch));

        _texture = new Texture2D(GraphicsDevice, 1, 1);
        _texture.SetData(new[] { Color.White });


        // Create service collection
        //var serviceCollection = new ServiceCollection();
        //serviceCollection.AddSingleton<SpriteBatch>();
        //var serviceProvider = serviceCollection.BuildServiceProvider();
        //_spriteBatch = serviceProvider.GetService<SpriteBatch>();

        base.Initialize();
    }

    protected override void LoadContent()
    {
        _fontSystem = new FontSystem();
        _fontSystem.AddFont(File.ReadAllBytes(@"Content/DejaVuSans.ttf"));
        _fontSystem.GetFont(18);

        LoadGrid();
    }

    public override void Draw(GameTime gameTime)
    {
        _spriteBatch.Begin();

        foreach (var tile in Configuration.Tiles)
        {
            if (tile == null) continue;

            _spriteBatch.Draw(
                _texture,
                new Rectangle(
                    new Point(tile.X + ScaleWidth, tile.Y + ScaleHeight),
                    new Point(ScaleWidth - 1, ScaleHeight - 1)),
                null,
                tile.Color
            );
        }

        _spriteBatch.End();

        base.Draw(gameTime);
    }

    private void LoadGrid()
    {
        Configuration.Tiles = new Tile[20, 20];

        // Load Tiles
        for (var y = 0; y < MapHeight; y++)
        {
            for (var x = 0; x < MapWidth; x++)
            {
                var tileGrid = _map[y, x];

                Color selectedColor;
                switch (tileGrid)
                {
                    case 1: // open
                        selectedColor = Color.Black;
                        break;
                    //case 0: // closed
                    //    selectedColor = Color.White;
                    //    continue;
                    case 5: // exit
                    case 8: // entrance
                        selectedColor = Color.Red;
                        break;
                    default:
                        continue;
                }

                Configuration.Tiles[x, y] = new Tile
                {
                    X = x * ScaleWidth,
                    Y = y * ScaleHeight,
                    Color = selectedColor,
                    Type = (Tile.Types)tileGrid
                };
            }
        }

        base.LoadContent();
    }

    //-------------------------------MemoryRender------------------------
    // draws whatever path may be stored in the memory
    //--------------------------------------------------------------------
    public void MemoryRender(SpriteBatch sb)
    {
        if (_texture == null)
        {
            _texture = new Texture2D(GraphicsDevice, 1, 1);
            _texture.SetData(new[] { Color.White });
        }

        sb.Begin();

        for (var y = 0; y < m_iMapHeight; ++y)
        {
            for (var x = 0; x < m_iMapWidth; ++x)
            { 
                if (_memory[y, x] == 1)
                {
                    sb.Draw(
                        _texture,
                        new Rectangle(
                            new Point(
                                x * ScaleWidth + ScaleWidth,
                                y * ScaleHeight + ScaleHeight),
                            new Point(ScaleWidth - 1, ScaleHeight - 1)),
                        null,
                        Color.LightGray
                    );
                }
            }
        }

        sb.End();
    }

    //----------------------------TestRoute-------------------------------
    //  given a decoded vector of directions and a map object representing
    //	Bob's memory, this function moves Bob through the maze as far as 
    //	he can go updating his memory as he moves.
    //--------------------------------------------------------------------
    public double TestRoute(IList<int> vecPath, ref Map bobs)
    {
        var posX = m_iStartX;
        var posY = m_iStartY;

        foreach (var nextDir in vecPath)
        {
            switch (nextDir) // check within bounds and that we can move
            {
                
                case 0: //North
                    if (posY - 1 < 0 || _map[posY - 1, posX] == 1)
                        break;
                    posY -= 1;
                    break;

                case 1: //South
                    if (posY + 1 >= m_iMapHeight || _map[posY + 1, posX] == 1)
                        break;
                    posY += 1;
                    break;

                case 2: //East
                    if (posX + 1 >= m_iMapWidth || _map[posY, posX + 1] == 1)
                        break;
                    posX += 1;
                    break;

                case 3: //West
                    if (posX - 1 < 0 || _map[posY, posX - 1] == 1)
                        break;
                    posX -= 1;
                    break;

            }//end switch

            // mark the route in the memory array
            bobs._memory[posY, posX] = 1;
        }

        //now we know the finish point of Bobs journey, let's assign
        //a fitness score which is proportional to his distance from
        //the exit
        var diffX = Math.Abs(posX - m_iEndX);
        var diffY = Math.Abs(posY - m_iEndY);

        //we add the one to ensure we never divide by zero. Therefore
        //a solution has been found when this return value = 1
        return 1 / (double)(diffX + diffY + 1);
    }

    //---------------------ResetMemory--------------------------
    //    resets the memory map to zeros
    //------------------------------------------------------------
    public void ResetMemory()
    {
        for (var y = 0; y < MapHeight; ++y)
        {
            for (var x = 0; x < MapWidth; ++x)
            {
                _memory[y, x] = 0;
            }
        }
    }

}