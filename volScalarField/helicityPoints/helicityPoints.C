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
    helicityPoints

Description
    prints the helicity at each x, y, and z location

\*---------------------------------------------------------------------------*/

#include "calc.H"
#include "fvc.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{

    IOobject helicityheader
    (
        "helicity",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (helicityheader.headerOk())
    {
	Info<<"Printing helicity point data to helicity"<<runTime.timeName()<<".dat file \n";
	ofstream dataFile;
	string fileName;
	fileName ="helicity"+ runTime.timeName() +".dat";
	dataFile.open(fileName.c_str());//open the data file
        int cellCount=0;
	int boundaryFaceCount = 0;

        dataFile<< "% x\t y\t z\t helicity\t boundary\n";
        volScalarField helicity(helicityheader, mesh);
	const volVectorField& centers = mesh.C();
	const surfaceVectorField& faceCenters = mesh.Cf();

	// print all face values
	forAll(helicity.boundaryField(),patchi){
	
	label nFaces = mesh.boundaryMesh()[patchi].size();
        if(!isA<emptyPolyPatch>(mesh.boundaryMesh()[patchi])){
			for(int facei = 0; facei<nFaces; facei++){
			dataFile<<" "<<faceCenters.boundaryField()[patchi][facei].x()<<"\t "<<faceCenters.boundaryField()[patchi][facei].y()<<"\t "<<faceCenters.boundaryField()[patchi][facei].z()<<"\t "<<helicity.boundaryField()[patchi][facei]<<"\t 1\n";
			boundaryFaceCount++;
			}
		}	
	}
	Info<<"\t wrote face data from "<<boundaryFaceCount<<" boundary faces"<<endl;

	// print the internal field to the file
	forAll(helicity, cellI){

	dataFile<<" "<<centers[cellI].x()<<"\t "<<centers[cellI].y()<<"\t "<<centers[cellI].z()<<"\t "<<helicity[cellI]<<"\t 0\n";

	cellCount++;

	}

	Info<<"\t wrote cell data from "<<cellCount<<" cells"<<endl;

	dataFile.close();//close the data file

    }
    else
    {
        Info<< "    No helicity" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
