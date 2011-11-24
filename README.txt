

String where;   // FIXM

For the update class: 
TODO: zjistit jak se zjisti vsechny prvky primarniho klice... 
      a dalsi veci ohledne cizich klicu pro vyhledani moznosti spojeni.
TODO: vypsat kolik radku bylo ovlivneno pri update a verbose?





ADING Image on s2 [100643, 100643]
2011-11-03 20:11:28: WARNING 304: Value is not a string
2011-11-03 20:11:28: WARNING 302: There is nothing to print (see other messages)
2011-11-03 20:11:28: WARNING 156: No sequence location specified
terminate called after throwing an instance of 'std::bad_alloc'
  what():  std::bad_alloc
Aborted


// ******************************************************************************************

Hi,

there is a rare bug when some follows "Putting an array value" in http://libpqtypes.esilo.com/man3/pqt-specs.html.

Some should initialize also
    arr.lbound[0] = 1;

else arr is not added to the param (rarely)
    PQputf(param, "%float4[]", &arr);
and a parameter is missing then.

Cheers,

Petr
