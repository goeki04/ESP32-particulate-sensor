#version 460 core

// Wir geben die Texturkoordinaten an den Fragment Shader weiter
out vec2 TexCoords;

void main() {
    // Diese mathematische Formel erzeugt 3 Punkte, die ein 
    // Dreieck aufspannen, welches den gesamten Bildschirm überdeckt.
    // gl_VertexID wird automatisch von 0 bis 2 hochgezählt.
    
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);

    // Die UV-Koordinaten liegen dann genau im Bereich [0, 1]
    TexCoords.x = (x + 1.0) * 0.5;
    TexCoords.y = (y + 1.0) * 0.5;

    gl_Position = vec4(x, y, 0.0, 1.0);
}