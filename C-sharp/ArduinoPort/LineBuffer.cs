using System;
using System.Collections.Generic;
using System.Linq;

namespace ArduinoCom
{
    class LineBuffer
    {
        public int MinimumLineSize { get; set; } = 1;
        public string Buffer { get; set; } = "";
        public string NewLineChar { get { return newLineArr[0]; }  set { newLineArr[0] = value; } }
        private string[] newLineArr = new string[] { "\r\n" };

        public LineBuffer()
        {

        }

        public LineBuffer(string newLineChar, int minLineSize)
        {
            NewLineChar = newLineChar;
            MinimumLineSize = minLineSize;
        }

        public void AddData(string rawData)
        {
            Buffer += rawData;
        }

        public IEnumerable<string> GetLines()
        {
            string[] chunks = Buffer.Split(newLineArr, StringSplitOptions.None);

            Buffer = "";

            if (chunks.Last() != "")
            {
                Buffer = chunks.Last();
                chunks[chunks.Length - 1] = "";
            }

            foreach (string line in chunks)
            {
                if (line.Length > MinimumLineSize)
                    yield return line;
            }
        }
    }
}
