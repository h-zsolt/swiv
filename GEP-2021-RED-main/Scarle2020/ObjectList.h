#pragma once

//to save you typing in all the headers of all the classes derived from the GameObject class lets put them all here
//This is REALLY not the most efficent way of doing this but it sort of works!

#include "gameobject.h"
#include "camera.h"
#include "Light.h"
#include "TPSCamera.h"
#include "Player.h"
#include "Terrain.h"
#include "CMOGO.h"
#include "Tree.h"

//VBGO examples
#include "VBGO.h"
#include "fileVBGO.h"
#include "VBCube.h"
#include "VBSpiked.h"
#include "VBspiral.h"
#include "VBPillow.h"
#include "VBSnail.h"
#include "MarchCubes.h"

//and 2-D ones here
#include "GameObject2D.h"
#include "ImageGO2D.h"
#include "TextGO2D.h"

//Sound Objects
#include <Audio/Sound.h>
#include "Loop.h"
#include <Audio/TestSound.h>
