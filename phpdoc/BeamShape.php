<html>
<head>
<title>Beam Shape</title>
<link rel="stylesheet" type="text/css" href="pythia.css"/>
<link rel="shortcut icon" href="pythia32.gif"/>
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

<form method='post' action='BeamShape.php'>

<h2>Beam Shape</h2>

The <?php $filepath = $_GET["filepath"];
echo "<a href='BeamParameters.php?filepath=".$filepath."' target='page'>";?>Beam Parameters</a>
page explains how you can set a momentum spread of the two incoming
beams, and a spread and offset for the location of the interaction
vertex. The spread is based on a simple parametrization in terms of
independent Gaussians, however, which is likely to be too primitive
for realistic applications. 

<p/>
It is therefore possible to define your own class, derived from the 
<code>BeamShape</code> base class, and hand it in to Pythia with the
<?php $filepath = $_GET["filepath"];
echo "<a href='ProgramFlow.php?filepath=".$filepath."' target='page'>";?>
<code>pythia.setBeamShapePtr( BeamShape*)</code></a> method. 
Below we describe what such a class has to do. An explicit toy example 
is shown in <code>main27.cc</code>.

<p/>
The <code>BeamShape</code> base class has a very simple structure.
It only has two virtual methods. The first, <code>init()</code>, is 
used for initialization. The second, <code>pick()</code>, selects
beam momentum and production vertex in the current event.

<p/>
The base-class <code>init()</code> method simply reads in the values 
stored in the <code>Settings</code> data base. You are free to  
write your own derived initialization routine, or use the existing one. 
In the latter case you can then give your own modified interpretation 
to the beam spread parameters defined there. 

<p/>
The two flags <code>Beams:allowMomentumSpread</code> and 
<code>Beams:allowVertexSpread</code> should not be tampered with,
however. These are checked elsewhere to determine whether the beam 
shape should be set or not, whereas the other momentum-spread
and vertex-spread parameters are local to this class.

<p/>
The <code>pick()</code> method is the key one to supply in the derived
class. Here you are free to pick whatever parametrization you desire
for beam momenta and vertex position, including correlations between
the two. At the end of the day, you should set
<br/><code>deltaPxA, deltaPyA, deltaPzA</code> for the three-momentum 
deviation of the first incoming beam, relative to the nominal values;
<br/><code>deltaPxB, deltaPyB, deltaPzB</code> for the three-momentum 
deviation of the second incoming beam, relative to the nominal values;
<br/><code>vertexX, vertexY, vertexZ, vertexT</code> for the 
production-vertex position and time.
<br/>These values will then be read out with the three base-class methods
<code>deltaPA()</code>, <code>deltaPB()</code> and <code>vertex()</code>.  

</body>
</html>

<!-- Copyright (C) 2008 Torbjorn Sjostrand -->
