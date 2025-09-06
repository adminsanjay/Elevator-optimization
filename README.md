# Elevator-optimization
Stopping elevators on empty floors? It 's a little inefficient but it pays off – in time, energy and frustration.

As part of a recent project with my wonderful team we set about researching ways to make elevators a little bit smarter with the help of a Raspberry Pi 3 and some sensor integration.

Our proof-of-concept integrates:
1 PIR sensor for movement
2 Ultrasonic sensor for distance
3 USB camera for face detection

The combined effect of them makes the system more effective at recognizing true human presence before deciding whether the elevator should stop; reducing the number of stops and increasing overall efficiency.

basically the Raspberry Pi gathers real-time data from all the sensors and uses simple computer vision algorithms to determine if a person is actually waiting — it 's a big step toward smarter, more intuitive infrastructure — especially for places where every second ( and watt) counts.

To visually display our output, we developed a simple website using HTML and connected it to our Raspberry Pi using Socket.IO to show the output in real time.
