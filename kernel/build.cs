using System;
using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Collections.Generic;

class Program
{
	static string PLATFORM = "x86";
	
	static string HAL_ROOT => "hal/" + PLATFORM + "/";
	
	static List<string> objects = new List<string>();
	
	static Dictionary<string, string> mapper = new Dictionary<string, string>()
	{
		{ ".c", "$(CC) $(CFLAGS) -c -o $@ $<" },
		{ ".S", "$(AS) $(ASFLAGS) -c -o $@ $<" },
	};
	
	static int Main(string[] args)
	{
		PLATFORM = args.FirstOrDefault();
		
		var paths = new [] {
			"src",
			HAL_ROOT + "src",
		};
		
		using(var sw = new StreamWriter("build/Makefile"))
		{
			sw.WriteLine("PLATFORM={0}", PLATFORM);
			
			sw.WriteLine("all: kernel");
			sw.WriteLine("\t");
			sw.WriteLine();
			
			sw.WriteLine("include ../common.mk");
			sw.WriteLine();
			
			sw.WriteLine("include ../hal/{0}/config.mk", PLATFORM);
			sw.WriteLine();
			
			foreach(var path in paths) {
				WriteFiles(sw, path);
			}
			
			sw.WriteLine(
				"kernel: {0}",
				string.Join(" ", objects));
			sw.WriteLine(
				"\t$(LD) $(LDFLAGS) -o $@ $^",
				PLATFORM);
		}
		return 0;
	}
	
	static void WriteFiles(TextWriter sw, string root)
	{
		var files = Directory.GetFiles(root);
		foreach(var file in files)
		{
			var ext = Path.GetExtension(file);
			if(mapper.ContainsKey(ext) == false)
				continue;
			
			var obj = GetObjectFileName(file);
			objects.Add(obj);
			sw.WriteLine(
				"{0}: ../{1}",
				obj,
				file);
			sw.WriteLine(
				"\t{0}",
				mapper[ext]);
		}
	}
	
	static string GetObjectFileName(string file)
	{
		file = file.Replace("/", "-");
		return Path.GetFileNameWithoutExtension(file) + ".o";
	}
}
