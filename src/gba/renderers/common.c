/* Copyright (c) 2013-2019 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <mgba/internal/gba/renderers/common.h>

#include <mgba/gba/interface.h>

int GBAVideoRendererCleanOAM(struct GBAObj* oam, struct GBAVideoRendererSprite* sprites, int offsetY, int masterHeight, int masterWidth, bool combinedObjSort) {
	int i;
	int p;
	int oamMax = 0;
	int maxP = 1;
	if (combinedObjSort) {
		maxP = 4;
	}
	for (p = 0; p < maxP; ++p) {
		for (i = 0; i < 128; ++i) {
			struct GBAObj obj;
			LOAD_16LE(obj.a, 0, &oam[i].a);
			LOAD_16LE(obj.b, 0, &oam[i].b);
			LOAD_16LE(obj.c, 0, &oam[i].c);
			if (combinedObjSort && GBAObjAttributesCGetPriority(obj.c) != p) {
				continue;
			}
			if (GBAObjAttributesAIsTransformed(obj.a) || !GBAObjAttributesAIsDisable(obj.a)) {
				int width = GBAVideoObjSizes[GBAObjAttributesAGetShape(obj.a) * 4 + GBAObjAttributesBGetSize(obj.b)][0];
				int height = GBAVideoObjSizes[GBAObjAttributesAGetShape(obj.a) * 4 + GBAObjAttributesBGetSize(obj.b)][1];
				int cycles = width;
				if (GBAObjAttributesAIsTransformed(obj.a)) {
					height <<= GBAObjAttributesAGetDoubleSize(obj.a);
					width <<= GBAObjAttributesAGetDoubleSize(obj.a);
					cycles = 10 + width * 2;
				}
				if (GBAObjAttributesAGetY(obj.a) >= masterHeight && GBAObjAttributesAGetY(obj.a) + height < 256) {
					continue;
				}
				if (GBAObjAttributesBGetX(obj.b) >= masterWidth && GBAObjAttributesBGetX(obj.b) + width < 512) {
					continue;
				}
				int y = GBAObjAttributesAGetY(obj.a) + offsetY;
				sprites[oamMax].y = y;
				sprites[oamMax].endY = y + height;
				sprites[oamMax].cycles = cycles;
				sprites[oamMax].obj = obj;
				sprites[oamMax].index = i;
				++oamMax;
			}
		}
	}
	return oamMax;
}
