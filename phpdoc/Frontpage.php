<html>
<head>
<title>Front</title>
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

<form method='post' action='Frontpage.php'>

<h1>PYTHIA 8</h1>

<h2>Welcome to PYTHIA - The Lund Monte Carlo!</h2>

<p/>
PYTHIA 8 is the successor to PYTHIA 6, rewritten from scratch in C++. 
With the release of PYTHIA 8.1 it now becomes the official "current" 
PYTHIA version, although PYTHIA 6.4 will be supported in  parallel 
with it for some time to come. Specifically, the new version has not 
yet been enough tested and tuned for it to have reached the same level 
of reliability as the older one. This testing will only happen if 
people begin to work with the program, however, which is why we 
encourage a gradual transition to the new version, starting now. 
There are some new physics features in PYTHIA 8.1, that would make 
use of it more attractive, but also some topics still missing, where 
6.4 would have to be used. Further, many obsolete features will not 
be carried over, so for some backwards compatibility studies again 
6.4 would be the choice.

<h2>Documentation</h2>

On these webpages you will find the up-to-date manual for PYTHIA 8.1.
Use the left-hand index to navigate this documentation of program 
elements, especially of all possible program settings. All parameters 
are provided with sensible default values, however, so you need only 
change those of relevance to your particular study, such as choice of 
beams, processes and phase space cuts. The pages also contain a fairly 
extensive overview of all methods available to the user, e.g. to study 
the produced events. What is lacking on these webpages is an overview, 
on the one hand, and an in-depth physics description, on the other.

<p/>
The overview can be found in the attached PDF file
<br/><a href="pythia8100.pdf" target="page"><b>A Brief Introduction
to PYTHIA 8.1</b></a>
<br/>T. Sj&ouml;strand, S. Mrenna and P. Skands, arXiv:0710.3820,
submitted to Computer Physics Communications.
<br/>You are strongly recommended to read this summary when you 
start out to learn how to use PYTHIA 8.1. 

<p/>
For the physics description we refer to the complete 
<br/><b>PYTHIA 6.4 Physics and Manual</b>
<br/>T. Sj&ouml;strand, S. Mrenna and P. Skands, JHEP05 (2006) 026,
<br/>which in detail describes the physics (largely) implemented also in 
PYTHIA 8, and also provides a more extensive bibliography than found
here.

<p/>
When you use PYTHIA 8.1, you should therefore cite both, e.g. like
<br/><b>T. Sj&ouml;strand, S. Mrenna and P. Skands, JHEP05 (2006) 026,
arXiv:0710.3820</b>.

<h2>Program Authors and Licence</h2>

<p/>
Main author: <b>Torbj&ouml;rn Sj&ouml;strand</b><br/> 
CERN/PH, CH-1211 Geneva, Switzerland, and<br/>
Department of Theoretical Physics, Lund University, 
SE-223 62 Lund, Sweden<br/>
phone: + 41 - 22 - 767 82 27, e-mail: torbjorn@thep.lu.se   

<p/>
Author: <b>Stephen Mrenna</b><br/> 
Computing Division, Simulations Group, 
Fermi National Accelerator Laboratory,<br/>
MS 234, Batavia, IL 60510, USA<br/>
phone: + 1 - 630 - 840 - 2556, e-mail: mrenna@fnal.gov

<p/>
Author: <b>Peter Skands</b><br/> 
CERN/PH, CH-1211 Geneva, Switzerland, and<br/>
Theoretical Physics Department,
Fermi National Accelerator Laboratory,<br/>
MS 106, Batavia, IL 60510, USA<br/> 
phone: + 41 - 22 - 767 24 59, e-mail: skands@fnal.gov 

<p/>
Makefiles, configure scripts and HepMC interface by <b>Mikhail Kirsanov</b>.
<br/>Conversion of XML files to PHP ones by <b>Ben Lloyd</b>.
<br/>Simple Makefile for Win32/NMAKE by <b>Bertrand Bellenot</b>.
<br/>Extended Higgs sector partly implemented by <b>Marc Montull</b>.
<br/>Parts of charm and bottom decay tables courtesy <b>DELPHI</b> and
<b>LHCb</b> collaborations.

<p/>
PYTHIA 8 is licensed under the 
<a href="../COPYING" target="page"><b>GNU General Public Licence 
version 2</b></a>.
<br/>Please respect the 
<a href="../GUIDELINES" target="page"><b>MCnet Guidelines</b></a>
for Event Generator Authors and Users.

<p/>
The program and the documentation is
Copyright &copy; 2007 Torbj&ouml;rn Sj&ouml;strand 
 

</body>
</html>

<!-- Copyright (C) 2007 Torbjorn Sjostrand -->