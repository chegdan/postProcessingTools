/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Application
    vorticity

Description
    Calculates and writes the vorticity of velocity field U.

    The -noWrite option just outputs the max/min values without writing
    the field.

\*---------------------------------------------------------------------------*/

#include "calc.H"
#include "fvc.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{
   // bool writeResults = !args.optionFound("noWrite");

    IOobject kheader
    (
        "k",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (kheader.headerOk())
    {

	Info<<"Printing k point data to k.dat file \n";
	ofstream dataFile;
	string fileName;
	fileName ="k.dat";
	dataFile.open(fileName.c_str());//open the data file
        int cellCount=0;

        dataFile<< "% x\t y\t z\t k" << "\n";
        volScalarField k(kheader, mesh);
	const volVectorField& centers = mesh.C();

	forAll(k, cellI){

	dataFile<<" "<<centers[cellI].x()<<"\t "<<centers[cellI].y()<<"\t "<<centers[cellI].z()<<"\t "<<k[cellI]<<"\n";

	cellCount++;

	}

	Info<<"\t wrote cell data from "<<cellCount<<" cells"<<endl;

	dataFile.close();//close the data file
    }
    else
    {
        Info<< "    No k" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
