using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Section_AI_1_1_Pathfinder.Components;

namespace Section_AI_1_1_Pathfinder
{
    public class Game1 : Game
    {
        private readonly GraphicsDeviceManager _graphics;
        private SpriteBatch _spriteBatch;

        private int ScreenWidth { get; set; } = 443;
        private int ScreenHeight { get; set; } = 270;

        public Game1()
        {
            Window.Title = "Pathfinder";
            IsMouseVisible = true;
            Content.RootDirectory = "Content";
            _graphics = new GraphicsDeviceManager(this)
            {
                PreferredBackBufferWidth = ScreenWidth,
                PreferredBackBufferHeight = ScreenHeight,
            };

            //IsFixedTimeStep = false;
            //_graphics.SynchronizeWithVerticalRetrace = false;

            _graphics.ApplyChanges();
        }

        protected override void Initialize()
        {
            _spriteBatch = new SpriteBatch(GraphicsDevice); // load to the service so it will be there for the components

            Services.AddService(typeof(SpriteBatch), _spriteBatch);
            Components.Add(new Map(this));
            Components.Add(new GeneticAlgorithm(this));

            base.Initialize();
        }

        protected override void LoadContent()
        {

            base.LoadContent();
        }

        protected override void Update(GameTime gameTime)
        {
            if (GamePad.GetState(PlayerIndex.One).Buttons.Back == ButtonState.Pressed || Keyboard.GetState().IsKeyDown(Keys.Escape))
                Exit();

            base.Update(gameTime);
        }

        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.White);

            base.Draw(gameTime);
        }
    }
}
