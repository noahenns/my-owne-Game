# Trailer Driver: Road Jobs

Ein kleines Desktop-/Browser-Spiel, inspiriert von Fernfahrer- und Landwirtschafts-Simulatoren: Du fährst **Autos mit Anhänger** statt LKW, steigst aus dem Wagen aus, nimmst Paletten-Aufträge an und schaltest später größere Anhänger frei.

> Hinweis: Der Projektname und die Fahrzeuge sind eigenständig. Das mitgelieferte Markenzeichnen liegt als `assets/logo.svg` vor und wird im Spiel auf Auto und Anhänger angezeigt.

## Features

- Fahrbares Auto mit Anhänger im Canvas-Spiel.
- Ein-/Aussteigen am Fahrzeug wie in Simulatoren: zu Fuß zur Tür gehen und einsteigen.
- Tastatursteuerung und Gamepad-/PS-Lenkrad-Unterstützung über die Browser Gamepad API.
- 2-Achs-Anhänger ab Level 1 verfügbar.
- 3-Achs-Anhänger ab Level 10 freischaltbar.
- Paletten-Aufträge mit Fortschrittssystem: ungefähr 2-3 Aufträge mit je 5 Paletten bringen ein Viertel Level.
- Erste Fahrzeugklasse orientiert sich optisch an einem älteren kantigen Kombi, spätere Fahrzeuge können ergänzt werden.
- Eigenes handgezeichnetes Markenzeichnen auf Auto und Anhänger.
- Firmenname wird beim Spielstart gewählt und als Aufschrift auf den Anhänger gemalt.
- GitHub-tauglich: einfach herunterladen und `index.html` öffnen.

## Spielen

1. Repository herunterladen oder klonen.
2. `index.html` im Browser öffnen.
3. Firmennamen eingeben und Spiel starten; der Name erscheint auf dem Anhänger.
4. Optional: Für einen lokalen Server im Projektordner ausführen:

```bash
python3 -m http.server 8080
```

Dann `http://localhost:8080` öffnen.

## Steuerung

### Im Auto

| Aktion | Tastatur |
| --- | --- |
| Gas | `W` / `Pfeil hoch` |
| Bremse/Rückwärts | `S` / `Pfeil runter` |
| Lenken | `A`/`D` oder Pfeile links/rechts |
| Aussteigen | `E` nahe Stillstand |
| Anhänger wechseln | `1` = 2 Achsen, `2` = 3 Achsen ab Level 10 |

### Zu Fuß

| Aktion | Tastatur |
| --- | --- |
| Laufen | `WASD` / Pfeiltasten |
| Einsteigen | `E` an der Fahrertür |
| Auftrag annehmen/abgeben | `E` an Depots |

### Gamepad / PS-Lenkrad

- Lenken: linke Achse oder Lenkrad-Achse.
- Gas/Bremse: Trigger oder vertikale Achse.
- Aktion/Einsteigen: Button `0`.
- Manche Lenkräder melden Achsen unterschiedlich; die Browser Gamepad API wird automatisch abgefragt.

## Ziel des Prototyps

Dieser Stand ist ein spielbarer Prototyp. Er enthält bewusst keine geschützten Marken, keine echten Fahrzeugmodelle und kein Euro-Truck-/Real-Truck-Material. Er ist als Grundlage gedacht, um später eigene Karten, Sounds, Menüs und Fahrzeuge zu ergänzen.
