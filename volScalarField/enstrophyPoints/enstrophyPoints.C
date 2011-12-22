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
    enstrophyPoints

Description
    Writes the x,y, and z location and value of enstrophy to the screen.

\*---------------------------------------------------------------------------*/

#include "calc.H"
#include "fvc.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{

    IOobject enstrophyheader
    (
        "enstrophy",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (enstrophyheader.headerOk())
    {
        Info<< "% x\t y\t z\t enstrophy" << endl;
        volScalarField enstrophy(enstrophyheader, mesh);
	const volVectorField& centers = mesh.C();

	forAll(enstrophy, cellI){

	Info<<" "<<centers[cellI].x()<<"\t "<<centers[cellI].y()<<"\t "<<centers[cellI].z()<<"\t "<<enstrophy[cellI]<<endl;

	}
    }
    else
    {
        Info<< "    No enstrophy" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
