/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2009 OpenCFD Ltd.
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
    plusPostRANS

Description

    calculates y+ and u+ fields for wall-bounded flows computed with
    one of the available low-Re RANS (no wall function!) turbulence 
    models. More specifically it

    :: 	calculates and outputs y+ (avg., min., max.) based on the 
	velocity gradient at the wall  

    ::	calculates and outputs the wall averaged friction velocity 

    ::  writes fields of y+ and U+ to the corresponding time directory

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "incompressible/singlePhaseTransportModel/singlePhaseTransportModel.H"
#include "RASModel.H"
#include "LESModel.H"
#include "nearWallDist.H"
#include "wallDist.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    timeSelector::addOptions();

    #include "setRootCase.H"
    #include "createTime.H"
    instantList timeDirs = timeSelector::select0(runTime, args);
    #include "createMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        fvMesh::readUpdateState state = mesh.readUpdate();

        wallDist y(mesh, true);

        if (timeI == 0 || state != fvMesh::UNCHANGED)
        {
            Info<< "Calculating wall distance\n" <<endl;
            Info<< "Writing wall distance to field " << y.name() << nl << endl;
            y.write();
        }

	#include "createFields.H"

        const fvPatchList& patches = mesh.boundary();
        
        dimensionedScalar uTauAvg("uTauAvg", dimVelocity, 0);

        scalar nPatch = 0;
                
	Info<< "Summary: " << nl << endl;

        forAll(patches, patchi)
        {
            const fvPatch& currPatch = patches[patchi];

            if (typeid(currPatch) == typeid(wallFvPatch))
            {
                yPlusTemp.boundaryField()[patchi] =
                    d[patchi]
                   *sqrt
                    (
                        RASModel->nu()().boundaryField()[patchi]
                       *mag(U.boundaryField()[patchi].snGrad())
                    )
                   /RASModel->nu()().boundaryField()[patchi];
                
		const scalarField& YpTemp = yPlusTemp.boundaryField()[patchi];

                uTau.boundaryField()[patchi] =
                    sqrt
                    (
		    RASModel->nu()
                   *mag(U.boundaryField()[patchi].snGrad())
                    );

                const fvPatchScalarField& uTauWall = uTau.boundaryField()[patchi];
                
                dimensionedScalar uTauTmp("uTauTmp", dimVelocity, average(uTauWall));
                
                uTauAvg += uTauTmp;

                nPatch ++;

                Info<< "  y+ for Patch " << patchi
                    << " named " << currPatch.name() << ":" 
                    << " min: " << min(YpTemp) << " max: " << max(YpTemp)
                    << " average: " << average(YpTemp) 
                    << " avgUGradWall: " <<  average(mag(U.boundaryField()[patchi].snGrad())) << nl << endl;
            }
        }

        uTauAvg /= nPatch; 
        
        Info << "  avg. friction velocity uTau is: "
             << uTauAvg.value() << " (averaged over " << nPatch << " wall(s))" << nl <<endl;

        yPlus = y.y() * uTauAvg / RASModel->nu();
        uPlus = U / uTauAvg;
        
        Info << "Writing yPlus and uPlus to corresponding fields." << nl <<endl;
        yPlus.write();
        uPlus.write();

    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
