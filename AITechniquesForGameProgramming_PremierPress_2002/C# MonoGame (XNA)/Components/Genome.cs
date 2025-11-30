using System;
using System.Collections.Generic;

namespace Section_AI_1_1_Pathfinder.Components;

public struct Genome
{
    public List<int> Bits = new() { 0 };
    public double Fitness = new();

    public Genome()
    {
        Fitness = 0;
    }

    public Genome(int numBits) // Randomly generate a new genome in the constructor. 
    {
        Random random = new();
        for (var i = 0; i < numBits - 1; i++)
        {
            Bits.Add(random.Next(0, 2)); // between 0 and 2 means either 0 or 1. Odd syntax, but this picks either a 0 or 1. 
        }
    }
}