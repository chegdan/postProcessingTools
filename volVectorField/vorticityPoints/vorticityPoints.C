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
    under the terms of the GNvorticity General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOvorticityT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICvorticityLAR PvorticityRPOSE.  See the GNvorticity General Public License
    for more details.

    You should have received a copy of the GNvorticity General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 vorticitySA

Application
    vorticityPoints

Description
    prints the vorticity at each x, y, and z location

\*---------------------------------------------------------------------------*/

#include "calc.H"
#include "fvc.H"
#include <fstream>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{

    IOobject vorticityheader
    (
        "vorticity",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (vorticityheader.headerOk())
    {
	Info<<"Printing vorticity point data to vorticity"<<runTime.timeName()<<".dat file \n";
	std::ofstream dataFile;
	string fileName;
	fileName ="vorticity"+ runTime.timeName() +".dat";
	dataFile.open(fileName.c_str());//open the data file
        int cellCount=0;
	int boundaryFaceCount = 0;

        dataFile<< "% x\t y\t z\t vorticityx\t vorticityy\t vorticityz\t boundary " << "\n";
        volVectorField vorticity(vorticityheader, mesh);
	const volVectorField& centers = mesh.C();
	const surfaceVectorField& faceCenters = mesh.Cf();

	// print all face values
	forAll(vorticity.boundaryField(),patchi){
	
	label nFaces = mesh.boundaryMesh()[patchi].size();
        if(!isA<emptyPolyPatch>(mesh.boundaryMesh()[patchi])){
			for(int facei = 0; facei<nFaces; facei++){
			dataFile<<" "<<faceCenters.boundaryField()[patchi][facei].x()<<"\t "<<faceCenters.boundaryField()[patchi][facei].y()<<"\t "<<faceCenters.boundaryField()[patchi][facei].z()<<"\t "<<vorticity.boundaryField()[patchi][facei].x()<<"\t "<<vorticity.boundaryField()[patchi][facei].y()<<"\t "<<vorticity.boundaryField()[patchi][facei].z()<<"\t 1\n";
			boundaryFaceCount++;
			}
		}	
	}
	Info<<"\t wrote face data from "<<boundaryFaceCount<<" boundary faces"<<endl;

	// print the internal field to the file
	forAll(vorticity, cellI){

	dataFile<<" "<<centers[cellI].x()<<"\t "<<centers[cellI].y()<<"\t "<<centers[cellI].z()<<"\t "<<vorticity[cellI].x()<<"\t "<<vorticity[cellI].y()<<"\t "<<vorticity[cellI].z()<<"\n";

	cellCount++;

	}

	Info<<"\t wrote cell data from "<<cellCount<<" cells"<<endl;

	dataFile.close();//close the data file

    }
    else
    {
        Info<< "    No vorticity" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
