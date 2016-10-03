using System;
using System.IO;
using System.Text;
using System.Collections.Generic;

class Program
{
	static int Main(string[] args)
	{
		var outfile = "root.pfs";
		var files = new List<string>();
		for(int i = 0; i < args.Length; i++)
		{
			if(args[i].StartsWith("-"))
			{
				switch(args[i])
				{
					case "-o":
					case "--output":
						outfile = args[++i];
						break;
				}
			}
			else
			{
				files.Add(args[i]);
			}
		}
		
		using(var fs = File.Open(outfile, FileMode.Create, FileAccess.Write))
		{
			using(var writer = new BinaryWriter(fs, Encoding.ASCII))
			{
				unchecked {
					writer.Write((Int32)0xD05E4ABC);
				}
				foreach(var file in files)
				{
					var name = Path.GetFileNameWithoutExtension(file);
					var ext  = Path.GetExtension(file).ToUpper();
					
					var fname = name + ext.ToUpper();
					var namedat = fname.ToCharArray();
					
					Array.Resize(ref namedat, 16);
					
					var data = File.ReadAllBytes(file);
					
					writer.Write((Int32)data.Length);
					writer.Write(namedat);
					writer.Write(data);
				}
				writer.Write((Int32)0);
			}
		}
		return 0;
	}
}