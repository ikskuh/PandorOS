LoadFS("ATA0.0")

"INIT.BAS" -> Str0

If Not Exists(Str0)
  Goto Create

Run(Str0)
Exit

Label Create
Print("Create '", Str0, "' in order to customize init sequence.")
Exit