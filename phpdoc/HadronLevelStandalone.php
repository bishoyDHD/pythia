<html>
<head>
<title>Hadron-Level Standalone</title>
</head>
<body>

<script language=javascript type=text/javascript>
function stopRKey(evt) {
var evt = (evt) ? evt : ((event) ? event : null);
var node = (evt.target) ? evt.target :((evt.srcElement) ? evt.srcElement : null);
if ((evt.keyCode == 13) && (node.type=="text"))
{return false;}
}

document.onkeypress = stopRKey;
</script>
<?php
if($_POST['saved'] == 1) {
if($_POST['filepath'] != "files/") {
echo "<font color='red'>SETTINGS SAVED TO FILE</font><br/><br/>"; }
else {
echo "<font color='red'>NO FILE SELECTED YET.. PLEASE DO SO </font><a href='SaveSettings.php'>HERE</a><br/><br/>"; }
}
?>

<form method='post' action='HadronLevelStandalone.php'>

<h2>Hadron-Level Standalone</h2>

The Les Houches Accord allows external process-level configurations
to be fed in, for subsequent parton-level and hadron-level generation
to be handled internally by PYTHIA. There is no correspondingly 
standardized interface if you have external events that have also 
been generated through the parton-level stage, so that only the 
hadron-level remains to be handled. A non-standard way to achieve this
exists, however, and can be useful both for real applications and
for various tests of the hadronization model on its own. 

<p/>
The key trick is to set the flag <code>ProcessLevel:all = off</code>.
When <code>pythia.next()</code> is called it then does not try to
generate a hard process, and therefore also cannot do anything on the
parton level. Instead only the <code>HadronLevel</code> methods are
called, to take the current content of the event record stored in
<code>pythia.event</code> as a starting point for any hadronization 
and decays that are allowed by the normal parameters of this step. 
Often the input would consist solely of partons grouped into colour 
singlets, but also (colour-singlet) particles are allowed.

<p/>
To set up all the parameters, a <code>pythia.init()</code> call has
to be used, without any arguments. In brief, the structure of the
main program therefore should be something like
<pre>
  Pythia pythia;                               // Declare generator.
  Event& event = pythia.event                  // Convenient shorthand.
  pythia.readString("ProcessLevel:all = off"); // The trick!
  pythia.init();                               // Initialization.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    // Insert filling of event here!
    pythia.next();                             // Do the hadron level.
  }
</pre> 
Of course this should be supplemented by analysis of events, error checks,
and so on, as for a normal PYTHIA run. The unique aspect is how to fill
the <code>event</code> inside the loop, before <code>pythia.next()</code> 
is called.

<h3>Input configuration</h3>

To set up a new configuration the first step is to throw away the current
one, with <code>event.reset()</code>. This routine will also reserve
the zeroth entry in the even record to represent the event as a whole. 

<p/>
With the <code>event.append(...)</code> methods a new entry is added at the
bottom of the current record, i.e. the first time it is called entry 
number 1 is filled, and so on. The <code>append</code>  method basically
exists in four variants, either without or with history information,
and with four-momentum provided either as a <code>Vec4</code> four-vector 
or as four individual components:
<pre>
  append( id, status, col, acol, p, m)
  append( id, status, col, acol, px, py, pz, e, m)
  append( id, status, mother1, mother2, daughter1, daughter2, col, acol, p, m)
  append( id, status, mother1, mother2, daughter1, daughter2, col, acol, px, py, pz, e, m)
</pre> 
The methods return the index at which the entry has been stored,
but normally you would not use this feature. 

<p/>
You can find descriptions of the input variables 
<?php $filepath = $_GET["filepath"];
echo "<a href='ParticleProperties.php?filepath=".$filepath."' target='page'>";?>here</a>. 
The PDG particle code <code>id</code> and the Les Houches Accord colour 
<code>col</code> and anticolour <code>acol</code> tags must be set
correctly. The four-momentum and mass have to be provided in units of GeV; 
if you omit the mass it defaults to 0. 

<p/>
The status code can normally be simplified, however; you only need to recall 
that positive numbers correspond to particles that are still around, while
negative numbers denote ones that already hadronized or decayed, so usually
<i>+-1</i> is good enough. When <code>pythia.next()</code> is called
those positive-status particles that hadronize/decay get the sign of the
status code flipped to negative but the absolute value is retained. The
new particles are added with normal PYTHIA status codes.

<p/>
For normal hadronization/decays in <code>pythia.next()</code> the
history encoded in the mother and daughter indices is not used. 
Therefore the first two <code>append</code> methods, which set all these 
indices vanishing, should suffice. The subsequent hadronization/decays 
will still be properly documented.

<p/>
The exception is when you want to include junctions in your string 
topology, i.e. have three string pieces meet. Then you must insert in
your event record the (decayed) particle that is the reason for the
presence of a junction, e.g. a baryon beam remnant from which several
valence quarks have been kicked out, or a neutralino that underwent a
baryon-number-violating decay. This particle must have as daughters 
the three partons that together carry the baryon number. 
 
<p/>
The sample program in <code>main21.cc</code> illustrates how you can work
with this facility, both for simple parton configurations and for more
complicated ones with junctions.

</body>
</html>

<!-- Copyright (C) 2007 Torbjorn Sjostrand -->