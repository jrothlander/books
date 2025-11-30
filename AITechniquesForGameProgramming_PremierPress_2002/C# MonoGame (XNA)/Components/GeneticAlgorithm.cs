using System;
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using System.IO;
using System.Linq;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using FontStashSharp;
using Vector2 = Microsoft.Xna.Framework.Vector2;

namespace Section_AI_1_1_Pathfinder.Components;

public class GeneticAlgorithm : DrawableGameComponent
{
    private List<Genome> m_vecGenomes = new(); // population of genomes
    //SGenome[] m_vecGenomes; 

    private const float CROSSOVER_RATE = 0.7f;
    private const float MUTATION_RATE = 0.001f; 
    private const int PopSize = 140;
    private const int ChromoLength = 140;
    private const int GeneLength = 2;

    private readonly int m_iPopSize;             // population size
    private double m_dCrossoverRate;
    private double m_dMutationRate;

    private int m_iChromoLength;                // bits per chromosome
    private int m_iGeneLength;                  // bits per gene
    private int m_iFittestGenome;
    private double m_dBestFitnessScore;
    private double m_dTotalFitnessScore;
    private int m_iGeneration;

    //we use another CBobsMap object to keep a record of 
    //the best route each generation as an array of visited
    //cells. This is only used for display purposes.
    private Map m_BobsBrain;

    //lets you know if the current run is in progress.
    private bool m_bBusy;

    private new Game Game { get; }
    private KeyboardState _keyState, _prevKeyState;
    private SpriteBatch _spriteBatch;
    private FontSystem _fontSystem;
    private SpriteFontBase _font18;

    private readonly Random _rand = new();
    private readonly Utilities _util = new();

    public GeneticAlgorithm(Game game) : base(game)
    {
        Game = game;

        m_iPopSize = PopSize;
        m_iChromoLength = ChromoLength;
        m_iGeneLength = GeneLength;
        m_dCrossoverRate = CROSSOVER_RATE;
        m_dMutationRate = MUTATION_RATE;
        m_dTotalFitnessScore = 0.0;
        m_iGeneration = 0;
        m_bBusy = false;
    }

    public override void Initialize()
    {
        _spriteBatch = (SpriteBatch)Game.Services.GetService(typeof(SpriteBatch));
        base.Initialize();
    }

    protected override void LoadContent()
    {
        _fontSystem = new FontSystem();
        _fontSystem.AddFont(File.ReadAllBytes(@"Content/DejaVuSans.ttf"));

        _font18 = _fontSystem.GetFont(18);

        base.LoadContent();
    }

    public override void Update(GameTime gameTime)
    {
        _keyState = Keyboard.GetState();

        if (_keyState.IsKeyDown(Keys.Enter))
            GameState.State = GameState.GameStates.Start;

        if (_keyState.IsKeyDown(Keys.Space) && _prevKeyState.IsKeyUp(Keys.Space))
            GameState.State = GameState.GameStates.Stop;

        switch (GameState.State)
        {
            case GameState.GameStates.Stop:
                m_bBusy = false;
                break;

            case GameState.GameStates.Start:
                CreateStartPopulation();
                m_bBusy = true;
                break;

            case GameState.GameStates.Play:
                Epoch();
                break;

            case GameState.GameStates.Title:
            case GameState.GameStates.Pause:
            default:
                break;
        }

        _prevKeyState = _keyState;

        base.Update(gameTime);
    }

    public override void Draw(GameTime gameTime)
    {
        _spriteBatch.Begin();

        _spriteBatch.DrawString(
            _font18,
            $"Generation: {m_iGeneration}",
            new Vector2(
                10,
                1),
            Color.Black);

        if (m_bBusy)
        {
            _spriteBatch.DrawString(
                _font18,
                "Space to stop",
                new Vector2(
                    170,
                    246),
                Color.Black);
        }
        else
        {
            _spriteBatch.DrawString(
                _font18,
                "Press Return to start a new game",
                new Vector2(
                    90,
                    246),
                Color.Black);
        }

        _spriteBatch.End();

        // Render Test Path
        m_BobsBrain?.MemoryRender(_spriteBatch);

        base.Draw(gameTime);
    }

    private void CreateStartPopulation()
    {
        //clear existing population
        m_vecGenomes.Clear();

        for (var i = 0; i < m_iPopSize; i++)
            m_vecGenomes.Add(new Genome(m_iChromoLength));

        //reset all variables
        m_iGeneration = 0;
        m_iFittestGenome = 0;
        m_dBestFitnessScore = 0;
        m_dTotalFitnessScore = 0;

        GameState.State = GameState.GameStates.Play;
    }

    public void Epoch()
    {
        UpdateFitnessScores();

        if (!m_bBusy) return;

        // Create a new population
        var newBabies = 0;

        // Create storage for the baby genomes 
        var vecBabyGenomes = new List<Genome>();

        while (newBabies < m_iPopSize)
        {
            // Select the parents
            var mum = RouletteWheelSelection();
            var dad = RouletteWheelSelection();

            // Crossover
            var baby1 = new Genome();
            var baby2 = new Genome();

            CrossOver(mum.Bits, dad.Bits, ref baby1.Bits, ref baby2.Bits);

            // Mutate
            Mutate(baby1.Bits);
            Mutate(baby2.Bits);

            // Add to new population
            vecBabyGenomes.Add(baby1);
            vecBabyGenomes.Add(baby2);

            newBabies += 2;
        }

        // Copy babies back into starter population
        m_vecGenomes = vecBabyGenomes;

        // Increment the generation counter
        ++m_iGeneration;
    }

    public Genome RouletteWheelSelection()
    {
        // This will basically ignore any of the genomes with a 0 fitness score, as well as really low scores. 

        //var fSlice = _util.RandFloat() * m_dTotalFitnessScore; // get a random floating-point number and modify it by the total fitness score. 
        var fSlice = _rand.NextDouble() * m_dTotalFitnessScore; // get a random floating-point number and modify it by the total fitness score. 
        var cfTotal = 0.0;
        var selectedGenome = 0;

        // Loop through genome population.
        for (var i = 0; i < m_iPopSize; ++i)
        {
            cfTotal += m_vecGenomes[i].Fitness; // Create a running total of fitness of each existing genome encountered in the population.

            if (cfTotal > fSlice) // once the running total fitness score exceeds the fSlice value, select the current genome.
            {
                selectedGenome = i;
                break;
            }
        }

        return m_vecGenomes[selectedGenome];// if there are no fitness scores set yet, it will select the first genome.
    }

    private void Mutate(IList<int> vecBits)
    {
        for (var curBit = 0; curBit < vecBits.Count; curBit++)
        {
            // flip this bit?
            if (_util.RandFloat() < m_dMutationRate)
            {
                if (vecBits[curBit] == 1) // We could make this easier if we used bool for the bits and not integers.
                    vecBits[curBit] = 0;
                else
                    vecBits[curBit] = 1;
            }
        } // next bit
    }

    private void CrossOver(List<int> mum, List<int> dad, ref List<int> baby1, ref List<int> baby2)
    {
        // In a GA, the crossover function equates to sexual reproduction in living populations, but
        // the model in code is much less complex because it does not track or handle sex, X/Y chromosome,
        // and other biological mechanisms. 

        // Initializes baby1 and baby2 from mum and dad. Then randomly selects a crossover point
        // The two for-loops them swap the bits from mum and dad as of the crossover point selected.
        // The first for-loop selects the mum for set of bits, before the crossover point and 
        // assigns mum's first bits to baby1 and dad's to baby2. The second for-loop takes the 
        // bits after the crossover point and reverse the pattern, passing dad's bits to baby1 and
        // mum's to baby2.
        //
        // Examples
        //---------------------
        // # - mum's code
        // * - dad's code
        // x - crossover point
        //---------------------
        //
        // mum   #####x#####
        // dad   *****x*****
        // 
        // baby1 #####*****
        // baby2 *****#####
        //---------------------
        //
        // mum   ##x########
        // dad   **x********
        //
        // baby1 ##********
        // baby2 ********##

        // Return parents as offspring dependent on the rate or if parents are the same
        if (_rand.NextDouble() > m_dCrossoverRate || mum == dad)
        //if (_util.RandFloat() > m_dCrossoverRate || mum == dad)
        {
            baby1 = mum;
            baby2 = dad;

            return;
        }

        // Determine a crossover point
        //var cp = _util.RandInt(0, m_iChromoLength - 1);
        var cp = _rand.Next(0, m_iChromoLength);

        // Swap bits
        for (var i = 0; i < cp; ++i)
        {
            baby1.Add(mum[i]);
            baby2.Add(dad[i]);
        }

        for (var i = cp; i < mum.Count - 1; ++i)
        {
            baby1.Add(dad[i]);
            baby2.Add(mum[i]);
        }
    }

    private void UpdateFitnessScores()
    {
        m_iFittestGenome = 0;
        m_dBestFitnessScore = 0;
        m_dTotalFitnessScore = 0;

        var bobsMap = new Map(Game);
        var tempMemory = new Map(Game);

        //update the fitness scores and keep a check on fittest so far
        for (var i = 0; i < m_iPopSize; ++i)
        {
            //decode each genomes chromosome into a vector of directions
            var directions = Decode(m_vecGenomes[i].Bits);

            //get it's fitness score
            var gen = m_vecGenomes.ElementAt(i);
            gen.Fitness = bobsMap.TestRoute(directions, ref tempMemory);
            m_vecGenomes[i] = gen;

            //update total
            m_dTotalFitnessScore += gen.Fitness;

            //if this is the fittest genome found so far, store results
            if (gen.Fitness > m_dBestFitnessScore)
            {
                m_dBestFitnessScore = gen.Fitness;
                m_iFittestGenome = i;
                m_BobsBrain = tempMemory;

                //Has Bob found the exit?
                if (Math.Abs(gen.Fitness - 1.0f) < .01)
                {
                    m_bBusy = false; //is so, stop the run
                    break;
                }
            }

            tempMemory = new Map(Game);
        }//next genome
    }

    private IList<int> Decode(IList<int> bits)
    {
        var directions = new List<int>();

        // step through the chromosome one gene at a time
        for (var gene = 0; gene < bits.Count - 1; gene += m_iGeneLength)
        {
            // get the gene at this position
            IList<int> thisGene = new List<int>();

            for (var bit = 0; bit < m_iGeneLength; ++bit)
            {
                thisGene.Add(bits[gene + bit]);
            }

            // convert to decimal and add to list of directions
            directions.Add(BinToInt(thisGene));
        }

        return directions;
    }

    public static int BinToInt(IList<int> vec)
    {
        // Not really sure what the original version was doing, but it works.
        // However, this really should done by simply left-shifting the bits into an integer.
        var data = 0;
        for (var n = 0; n < vec.Count; n += 1)
            data = (data << 1) + (vec[n] == 1 ? 1 : 0);
        return data;
    }

}