using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Section_AI_1_1_Pathfinder
{
    public class Utilities
    {

        private readonly Random _random = new();
        private const int RAND_MAX = 0x7fff;

        public int RandInt(int x, int y)
        {
            return _random.Next(x, y) % (y - x + 1) + x;
        }

        public double RandFloat()
        {
            return _random.NextDouble() / (RAND_MAX + 1.0);
        }

        //public bool RandBool()
        //{
        //    return Convert.ToBoolean(RandInt(0, 1));
        //}

        //public double RandClamped()
        //{
        //    return RandFloat() - RandFloat();
        //}

    }
}
