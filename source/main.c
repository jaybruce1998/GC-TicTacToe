#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
 
#define DEFAULT_FIFO_SIZE	(256*1024)
 
static void *frameBuffer[2] = { NULL, NULL};
GXRModeObj *rmode;

// Define spacing and grid offsets
f32 spacing = 0.2f; // Space between quads
f32 quad_size = 1.0f; // Size of each quad
f32 grid_start_x = -2.0f; // Starting X position of the grid
f32 grid_start_y = 2.0f;  // Starting Y position of the grid
char board[3][3];
char turn, winner;
int cur;
void drawGrid(Mtx view) {
	Mtx model, modelview;
	// Draw 3x3 grid of quads
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			// Calculate position of each quad
			f32 posX = grid_start_x + col * (quad_size + spacing);
			f32 posY = grid_start_y - row * (quad_size + spacing);

			// Create transformation matrix for the current quad
			guMtxIdentity(model);
			guMtxTransApply(model, model, posX, posY, -6.0f); // Z = -6.0 to bring it into view
			guMtxConcat(view, model, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xA5, 0xA5, 0x56, 0x00 });

			// Draw the quad
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			
				GX_Position3f32(-quad_size / 2, quad_size / 2, 0.0f);	// Top Left
				GX_Position3f32( quad_size / 2, quad_size / 2, 0.0f);	// Top Right
				GX_Position3f32( quad_size / 2,-quad_size / 2, 0.0f);	// Bottom Right
				GX_Position3f32(-quad_size / 2,-quad_size / 2, 0.0f);	// Bottom Left
			GX_End();
		}
	}
}

void drawCursor(Mtx view, int row, int col) {
    Mtx model, modelview;
    f32 cursorThickness = 0.1f;  // Thin line thickness for the cursor
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    // Top horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + quad_size / 2, -6.0f); // Top of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0xFF, 0x00, 0xFF }); //Green

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);  // Top Left
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);  // Top Right
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Right
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Left
    GX_End();

    // Bottom horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY - quad_size / 2, -6.0f); // Bottom of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);  // Top Left
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);  // Top Right
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Right
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Left
    GX_End();

    // Left vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX - quad_size / 2, posY, -6.0f); // Left side of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-cursorThickness / 2,  quad_size / 2, 0.0f); // Top Left
        GX_Position3f32( cursorThickness / 2,  quad_size / 2, 0.0f); // Top Right
        GX_Position3f32( cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Right
        GX_Position3f32(-cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Left
    GX_End();

    // Right vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX + quad_size / 2, posY, -6.0f); // Right side of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-cursorThickness / 2,  quad_size / 2, 0.0f); // Top Left
        GX_Position3f32( cursorThickness / 2,  quad_size / 2, 0.0f); // Top Right
        GX_Position3f32( cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Right
        GX_Position3f32(-cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Left
    GX_End();
}

void drawX(Mtx view, int row, int col) {
    Mtx model, modelview;
    guVector axis;
    f32 cursorThickness = 0.1f;  // Thickness for the lines
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    // First diagonal line
    guMtxIdentity(model);
    
    axis.x = 0;
    axis.y = 0;
    axis.z = 1.0f;  // Rotate around the Z-axis to create diagonal
    guMtxRotAxisDeg(model, &axis, 45.0f);  // 45-degree rotation

    guMtxTransApply(model, model, posX, posY, -6.0f);  // Apply translation
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0x14, 0x93, 0xFF }); // Pink color
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f);
    GX_End();

    // Second diagonal line (opposite direction)
    guMtxIdentity(model);

    axis.x = 0;
    axis.y = 0;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, -45.0f);  // Opposite 45-degree rotation

    guMtxTransApply(model, model, posX, posY, -6.0f);  // Apply translation
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f);
    GX_End();
}

void drawO(Mtx view, int row, int col) {
    Mtx model, modelview;
    int numSegments = 40;  // Number of segments for the circle
    f32 radiusOuter = quad_size / 2;  // Outer radius for the "O"
    f32 radiusInner = radiusOuter - 0.2f;  // Inner radius for the "O"
    f32 angleStep = 2 * M_PI / numSegments;  // Step size for each segment
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);  // Position the O
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x80, 0x00, 0x80, 0xFF }); // Purple color
    
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
    f32 radiusOuter = quad_size / 2;  // Outer radius for the "O"
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
    float lineLength = quad_size;   // Length of the lines
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

// Function to display "X WINS"
void drawWon(Mtx view) {
    float startX = -2.5f; // Starting X position for "X WINS"
    float startY = -1.5f; // Y position to place the message below the board
    float spacing = 0.7f; // Spacing between the letters

    drawLetterW(view, startX+spacing*2, startY);
    drawLetterO(view, startX + 4 * spacing, startY);
    drawLetterN(view, startX + 5.5 * spacing, startY);
}

void setupBoard() {
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			board[i][j]=0;
	turn='x';
	winner='w';
	cur=1;
}

int main( int argc, char **argv ){
	f32 yscale;

	u32 xfbHeight;

	Mtx view;
	Mtx44 perspective;
	u32	fb = 0; 	// initial framebuffer index
	GXColor background = {0, 0, 0, 0};

	// init the vi.
	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);
	PAD_Init();
	
	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	// setup the fifo and then init the flipper
	void *gp_fifo = NULL;
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);
 
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
 
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);
 
	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
 
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 

	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
 
	// setup the vertex attribute table
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
 
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0xFF, 0xFF, 0 });
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 0.0F, 0.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
 

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 45 degrees,
	// and aspect ratio based on the display resolution
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
	guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	
 	setupBoard();
	int cursorRow = 0, cursorCol = 0;  // Track cursor position on the grid
	int moved = 0, movedBefore = 0;
	while(1) {
		PAD_ScanPads();
		
		u16 buttons = PAD_ButtonsDown(0);    // Read button press
		u16 buttonsHeld = PAD_ButtonsHeld(0); // Read button hold (for control stick)

		if (buttons & PAD_BUTTON_START && winner!='w') {
		    setupBoard();
		}
		if (buttons & PAD_BUTTON_A && board[cursorRow][cursorCol] == 0 && winner=='w') {
			board[cursorRow][cursorCol]=turn;
			if((board[0][cursorCol]==turn&&board[1][cursorCol]==turn&&board[2][cursorCol]==turn)||(board[cursorRow][0]==turn&&board[cursorRow][1]==turn&&board[cursorRow][2]==turn)||(board[1][1]==turn&&
				((board[0][0]==turn&&board[2][2]==turn)||(board[0][2]==turn&&board[2][0]==turn))))
				winner=turn;
			else if(cur++==9)
				winner='t';
			else
				turn=turn=='x'?'o':'x';
		}

		// Handle control stick input
		s8 stickX = PAD_StickX(0);  // Horizontal axis (-128 to 127)
		s8 stickY = PAD_StickY(0);  // Vertical axis (-128 to 127)

		// Move the cursor left/right
		if(movedBefore == 0)
		{
			if (stickX > 30) {  // Stick moved right
				cursorCol=cursorCol==2?0:cursorCol+1;
				moved=1;
			} else if (stickX < -30) {  // Stick moved left
				cursorCol=cursorCol==0?2:cursorCol-1;
				moved=1;
			}

			// Move the cursor up/down
			if (stickY > 30) {  // Stick moved up
				cursorRow=cursorRow==0?2:cursorRow-1;
				moved=1;
			} else if (stickY < -30) {  // Stick moved down
				cursorRow=cursorRow==2?0:cursorRow+1;
				moved=1;
			}
			movedBefore = moved;
		}
		if(abs(stickX) < 30 && abs(stickY) < 30) {
			movedBefore=0;
		}
		// Do this before drawing
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);

		drawGrid(view);  // Draw the Tic-Tac-Toe grid
		drawCursor(view, cursorRow, cursorCol);  // Draw the cursor at current position
		switch(winner)
		{
			case 'x':
    			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0x14, 0x93, 0xFF }); // Pink color
				drawLetterX(view, -2.5f, -1.5f);
				drawWon(view);
				break;
			case 'o':
				GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x80, 0x00, 0x80, 0xFF }); // Purple color
				drawLetterO(view, -2.5f, -1.5f);
				drawWon(view);
				break;
			case 't':
				GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0x00, 0xFF, 0xFF }); // Blue color
				drawLetterT(view, -2.5f, -1.5f);
				drawLetterI(view, -1.4f, -1.5f);
				drawLetterE(view, -0.3f, -1.5f);
		}
		// Draw X in the corresponding cells
		for (int row = 0; row < 3; row++) {
		    for (int col = 0; col < 3; col++) {
		        if (board[row][col] == 'x') {
		            drawX(view, row, col);  // Draw an X
		        }
		        else if(board[row][col] == 'o') {
		        	drawO(view, row, col);
		        }
		    }
		}
		// Do this stuff after drawing
		GX_DrawDone();

		fb ^= 1;  // Flip framebuffer
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb], GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
	}

	return 0;
}

