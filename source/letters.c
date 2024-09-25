#include <gccore.h>
#include <math.h>

void drawLetterX(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 1.0f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be

    // First diagonal line (top-left to bottom-right)
    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, 45); // Rotate to diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position the X on the screen
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();

    // Second diagonal line (bottom-left to top-right)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -45); // Rotate to opposite diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterW(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 1.0f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be
    float offset = 0.25f;    // Adjust the offset to align the lines properly

    // First diagonal line of the first V (top-left to bottom-middle)
    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, 15); // Rotate to the left diagonal
    guMtxTransApply(model, model, posX - offset, posY, -6.0f); // Position to the left side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Second diagonal line of the first V (bottom-middle to top-right)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -15); // Rotate to the right diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // First diagonal line of the second V (top-right to bottom-middle)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, 15); // Rotate to create the right side of the W
    guMtxTransApply(model, model, posX + offset, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -15); // Rotate to the right diagonal
    guMtxTransApply(model, model, posX + offset*2, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();
}

void drawLetterO(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    int numSegments = 40;  // Number of segments for the circle
    f32 radiusOuter = 0.5f;
    f32 radiusInner = radiusOuter - 0.2f;  // Inner radius for the "O"
    f32 angleStep = 2 * M_PI / numSegments;  // Step size for each segment

    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);  // Position the O
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    // Draw outer ring
    GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 2 * numSegments + 2);
    for (int i = 0; i <= numSegments; i++) {
        f32 angle = i * angleStep;
        f32 xOuter = radiusOuter * cos(angle);
        f32 yOuter = radiusOuter * sin(angle);
        f32 xInner = radiusInner * cos(angle);
        f32 yInner = radiusInner * sin(angle);

        // Outer vertex
        GX_Position3f32(xOuter, yOuter, 0.0f);

        // Inner vertex
        GX_Position3f32(xInner, yInner, 0.0f);
    }
    GX_End();
}

void drawLetterN(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 1.0f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be
    float offset = 0.25f;    // Offset for vertical lines

    // Left vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX - offset, posY, -6.0f); // Position to the left side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Right vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX + offset, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, -65); // Rotate to diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position the X on the screen
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterT(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float quad_size = 1.0f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be
    float offset = 0.5f;    // Offset for horizontal lines
    
    // Vertical line (the body of the T)
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position the I on the screen
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Top horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + quad_size / 2, -6.0f); // Position above the vertical line
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, -thickness / 2, 0.0f);
        GX_Position3f32(-offset, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterI(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float quad_size = 1.0f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be
    float offset = 0.5f;    // Offset for horizontal lines

	drawLetterT(view, posX, posY);
    // Bottom horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY - quad_size / 2, -6.0f); // Position below the vertical line
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, -thickness / 2, 0.0f);
        GX_Position3f32(-offset, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterE(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float lineLength = 1.0f;   // Length of the lines
    float thickness = 0.1f;         // Thickness of the lines
    
    // Vertical line of E (left side)
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX - lineLength / 2, posY, -6.0f);  // Position on the left side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -lineLength / 2, 0.0f);
        GX_Position3f32(thickness / 2, -lineLength / 2, 0.0f);
        GX_Position3f32(thickness / 2, lineLength / 2, 0.0f);
        GX_Position3f32(-thickness / 2, lineLength / 2, 0.0f);
    GX_End();

    // Top horizontal line of E
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + lineLength / 2, -6.0f); // Position at the top
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, thickness / 2, 0.0f);
        GX_Position3f32(-lineLength / 2, thickness / 2, 0.0f);
    GX_End();

    // Middle horizontal line of E
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position in the middle
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, thickness / 2, 0.0f);
        GX_Position3f32(-lineLength / 2, thickness / 2, 0.0f);
    GX_End();

    // Bottom horizontal line of E
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY - lineLength / 2, -6.0f); // Position at the bottom
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, -thickness / 2, 0.0f);
        GX_Position3f32(lineLength / 2, thickness / 2, 0.0f);
        GX_Position3f32(-lineLength / 2, thickness / 2, 0.0f);
    GX_End();
}
