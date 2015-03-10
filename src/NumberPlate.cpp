#include "NumberPlate.h"

NumberPlate::NumberPlate()
{
    //ctor
}

NumberPlate::~NumberPlate()
{
    //dtor
}

void NumberPlate::ProcessNumberPlate(){

/*
    //ImageToBW(100, true); // Make the charactors white
    ImageToLocalisedMonochrome();
    CreateIntensityRowProfileX(true); // Create RowProfileX for the licences plate;
    */
    SaveImageToFile("output.bmp");

}

void NumberPlate::RefinePlateY(){

//    CreateRowProfileY();

}

void NumberPlate::SegmentCharactors(){



}
