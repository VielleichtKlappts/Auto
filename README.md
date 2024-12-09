Programm um ein kleines Auto innerhalb des eigenen Wifis fernzusteuern.

Version 1 besteht aus einem Carrera Servo Auto bei welchem der Motor und die Steuerung (Servo) von einem ArduinoR4Wifi betrieben wird und vorne eine ESP32 Kamera hat welches ein video streamt. 

Das Python Script läuft auf dem mac und erlaubt einem hier das Auto mit der Maus oder per Tastatur über ein Pygame window zu steuern.
Im Hintergrund des Pygame interfaces versucht das script den ESP-Cam Stream zu laden.

Der Arduino muss an sich nicht sein, aber der hat(te) nen spannungsregler um mit den 7.2V meines Akkupacks zu arbeiten.

Leider ist der Arduino VinPin Kaputt gegangen weshalb er jetzt nur noch über kabel mit strom versorgt werden kann.

Spannungsregler ist aber bestellt, bald kann der Arduino also komplett weg, dann läuft alles über die ESP Cam und das Auto kann endlich ohne Kabel fahren.
