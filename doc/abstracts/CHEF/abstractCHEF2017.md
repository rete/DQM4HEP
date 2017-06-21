CHEF2017 
Online Data Quality Monitoring (DQM) is a central tool to the good operation of High Energy Physics (HEP) experiments. Most of these experiments develop their own DQM solutions on top of the Event Data Model (EDM) specific to their detector. This leads to a strong dependency on the data format and make the reusability of the DQM tools for other detectors difficult.
We present here a generic online monitoring framework, independent of the EDM and data type, called DQM4HEP. It is designed for experiments ranging from tabletop size to large multi-detector test beams set-ups. 
To illustrate the flexibility of these tools, dedicated implementations, including physics analysis, for three of the [CALICE][1] collaboration prototypes were tested during test beam campains at CERN SPS and DESY2 facilities. The AIDA-2020 project chose this framework for the monitoring of all their detector prototype.

[1]: CAlorimeter for LInear Collider Experiment 