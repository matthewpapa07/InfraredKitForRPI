using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IrDataDecoder
{
    class Program
    {
        static void Main(string[] args)
        {
            int counter = 0;
            string line;
            List<string> OriginalDoc = new List<string>(1000);

            // Read the file and display it line by line.
            System.IO.StreamReader file =
                new System.IO.StreamReader(@"C:\dev\PiTrunk\PiProjects\IrDataDecoder\IrDataDecoder\output1.txt");
            while ((line = file.ReadLine()) != null)
            {
                line.Trim();
//                System.Console.WriteLine(line);
                OriginalDoc.Add(line);
                counter++;
            }

            file.Close();
            System.Console.WriteLine("There were {0} lines.", counter);

            // first value is baud rate.
            int singleBitTime = Int32.Parse(OriginalDoc[0]);
            List<bool> bitArray = new List<bool>(1000);
            bool currentValue = true;
            double readValue;

            // Extract numbers, descipher bit values
            for (int i = 2; i < OriginalDoc.Count; i++)
            {
                readValue = Int32.Parse(OriginalDoc[i]);
                readValue /= singleBitTime;
                readValue = Math.Round(readValue);

                for (int j = 0; j < readValue; j++)
                {
                    bitArray.Add(currentValue);
                }
                currentValue = !currentValue;
            }

            // Align bit array to byte boundary. Pad with zeros if necessary
            for (int i = 0; i < bitArray.Count % 8; i++)
            {
                bitArray.Add(false);
            }

            // Pack results
            byte[] Result = new byte[bitArray.Count / 8];
            for (int i = 0; i < bitArray.Count / 8; i++)
            {
                Result[i] += (byte)(128 * boolToBit(bitArray[8 * i + 0]));
                Result[i] += (byte)(64  * boolToBit(bitArray[8 * i + 1]));
                Result[i] += (byte)(32  * boolToBit(bitArray[8 * i + 2]));
                Result[i] += (byte)(16  * boolToBit(bitArray[8 * i + 3]));
                Result[i] += (byte)(8   * boolToBit(bitArray[8 * i + 4]));
                Result[i] += (byte)(4   * boolToBit(bitArray[8 * i + 5]));
                Result[i] += (byte)(2   * boolToBit(bitArray[8 * i + 6]));
                Result[i] += (byte)(1   * boolToBit(bitArray[8 * i + 7]));
            }

            string outStr = ByteArrayToString2(Result);
            System.Console.WriteLine(outStr);

            System.IO.File.Delete(@"C:\dev\PiTrunk\PiProjects\IrDataDecoder\IrDataDecoder\OutBits.h");

            List<string> OutputLines = new List<string>(1000);
            OutputLines.Add("// This is an automatically generated file by irDataDecoder\n");
            OutputLines.Add("//" + outStr + "\n");
            OutputLines.Add("\n");
            OutputLines.Add("#define OUTBITS_LENGTH  " + bitArray.Count);
            OutputLines.Add("\n");
            OutputLines.Add("unsigned char OutBits[] = { \n");

            string ActualBits = "";
            for (int i = 0; i < bitArray.Count; i++)
            {
                if ((i % 20) == 0)
                    ActualBits += "\n";
                ActualBits += boolToStr(bitArray[i]) + ",";
            }
            OutputLines.Add(ActualBits + "\n 0 \n };");

            System.IO.File.WriteAllLines(@"C:\dev\PiTrunk\PiProjects\IrDataDecoder\IrDataDecoder\OutBits.h", OutputLines.ToArray());

        }

        public static byte boolToBit(bool input)
        {
            if (input)
                return 1;
            else
                return 0;
        }

        public static string boolToStr(bool input)
        {
            if (input)
                return "1";
            else
                return "0";
        }

        public static string ByteArrayToString(byte[] ba)
        {
            StringBuilder hex = new StringBuilder(ba.Length * 2);
            foreach (byte b in ba)
                hex.AppendFormat("{0:x2}", b);
            return hex.ToString();
        }

        public static string ByteArrayToString2(byte[] ba)
        {
            string hex = BitConverter.ToString(ba);
            return hex.Replace("-", "");
        }
    }
}
