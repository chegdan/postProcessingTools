/*---------------------------------------------------------------------------* \
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    Bo

Description
    Calculates and writes the Bo number as a surfaceScalarField obtained from
    field phi.

    The -noWrite option just outputs the max/min values without writing
    the field.

\*---------------------------------------------------------------------------*/

#include "calc.H"
#include "fvc.H"

#include "incompressible/singlePhaseTransportModel/singlePhaseTransportModel.H"
#include "incompressible/RAS/RASModel/RASModel.H"
#include "incompressible/LES/LESModel/LESModel.H"
#include "basicThermo.H"
#include "compressible/RAS/RASModel/RASModel.H"
#include "compressible/LES/LESModel/LESModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::calc(const argList& args, const Time& runTime, const fvMesh& mesh)
{
    bool writeResults = !args.optionFound("noWrite");

    IOobject phiHeader
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ
    );

    if (phiHeader.headerOk())
    {
        autoPtr<surfaceScalarField> BoPtr;

        Info<< "    Reading phi" << endl;
        surfaceScalarField phi(phiHeader, mesh);

        volVectorField U
        (
            IOobject
            (
                "U",
                runTime.timeName(),
                mesh,
                IOobject::MUST_READ
            ),
            mesh
        );

        IOobject RASPropertiesHeader
        (
            "RASProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        );

        IOobject LESPropertiesHeader
        (
            "LESProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        );

        Info<< "    Calculating Bo" << endl;

        if (phi.dimensions() == dimensionSet(0, 3, -1, 0, 0))
        {
            if (RASPropertiesHeader.headerOk())
            {
                IOdictionary RASProperties(RASPropertiesHeader);

                singlePhaseTransportModel laminarTransport(U, phi);

                autoPtr<incompressible::RASModel> RASModel
                (
                    incompressible::RASModel::New
                    (
                        U,
                        phi,
                        laminarTransport
                    )
                );

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mag(phi)
                       /(
                            mesh.magSf()
                          * mesh.surfaceInterpolation::deltaCoeffs()
                          * fvc::interpolate(RASModel->nuEff())
                         )
                    )
                );
            }
            else if (LESPropertiesHeader.headerOk())
            {
                IOdictionary LESProperties(LESPropertiesHeader);

                singlePhaseTransportModel laminarTransport(U, phi);

                autoPtr<incompressible::LESModel> sgsModel
                (
                    incompressible::LESModel::New(U, phi, laminarTransport)
                );

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mag(phi)
                       /(
                            mesh.magSf()
                          * mesh.surfaceInterpolation::deltaCoeffs()
                          * fvc::interpolate(sgsModel->nuEff())
                        )
                    )
                );
            }
            else
            {
                IOdictionary transportProperties
                (
                    IOobject
                    (
                        "transportProperties",
                        runTime.constant(),
                        mesh,
                        IOobject::MUST_READ_IF_MODIFIED,
                        IOobject::NO_WRITE
                    )
                );

                dimensionedScalar nu(transportProperties.lookup("D"));

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mesh.surfaceInterpolation::deltaCoeffs()
                      * (mag(phi)/mesh.magSf())*(runTime.deltaT()/nu)
                    )
                );
            }
        }
        else if (phi.dimensions() == dimensionSet(1, 0, -1, 0, 0))
        {
            if (RASPropertiesHeader.headerOk())
            {
                IOdictionary RASProperties(RASPropertiesHeader);

                autoPtr<basicThermo> thermo(basicThermo::New(mesh));

                volScalarField rho
                (
                    IOobject
                    (
                        "rho",
                        runTime.timeName(),
                        mesh
                    ),
                    thermo->rho()
                );

                autoPtr<compressible::RASModel> RASModel
                (
                    compressible::RASModel::New
                    (
                        rho,
                        U,
                        phi,
                        thermo()
                    )
                );

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mag(phi)
                       /(
                            mesh.magSf()
                          * mesh.surfaceInterpolation::deltaCoeffs()
                          * fvc::interpolate(RASModel->muEff())
                        )
                    )
                );
            }
            else if (LESPropertiesHeader.headerOk())
            {
                IOdictionary LESProperties(LESPropertiesHeader);

                autoPtr<basicThermo> thermo(basicThermo::New(mesh));

                volScalarField rho
                (
                    IOobject
                    (
                        "rho",
                        runTime.timeName(),
                        mesh
                    ),
                    thermo->rho()
                );

                autoPtr<compressible::LESModel> sgsModel
                (
                    compressible::LESModel::New(rho, U, phi, thermo())
                );

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mag(phi)
                       /(
                            mesh.magSf()
                          * mesh.surfaceInterpolation::deltaCoeffs()
                          * fvc::interpolate(sgsModel->muEff())
                        )
                    )
                );
            }
            else
            {
                IOdictionary transportProperties
                (
                    IOobject
                    (
                        "transportProperties",
                        runTime.constant(),
                        mesh,
                        IOobject::MUST_READ_IF_MODIFIED,
                        IOobject::NO_WRITE
                    )
                );

                dimensionedScalar mu(transportProperties.lookup("gamma"));

                BoPtr.set
                (
                    new surfaceScalarField
                    (
                        IOobject
                        (
                            "Bo",
                            runTime.timeName(),
                            mesh,
                            IOobject::NO_READ
                        ),
                        mesh.surfaceInterpolation::deltaCoeffs()
                      * (mag(phi)/(mesh.magSf()))*(runTime.deltaT()/mu)
                    )
                );
            }
        }
        else
        {
            FatalErrorIn(args.executable())
                << "Incorrect dimensions of phi: " << phi.dimensions()
                    << abort(FatalError);
        }


        // can also check how many cells exceed a particular Bo limit
        /*
        {
            label count = 0;
            label BoLimit = 200;
            forAll(BoPtr(), i)
            {
                if (BoPtr()[i] > BoLimit)
                {
                    count++;
                }

            }

            Info<< "Fraction > " << BoLimit << " = "
                << scalar(count)/Bo.size() << endl;
        }
        */

        Info<< "Bo max : " << max(BoPtr()).value() << endl;

	/*Info<< "Writing Cell Bodenstein Number BoCell" << endl;
	
        volScalarField BoCell
        (
            IOobject
            (
                "BoCell",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ
            ),
            mesh,
            dimensionedScalar("0", dimless, 0),
            zeroGradientFvPatchScalarField::typeName
        );

	BoCell.dimensionedInternalField() = fvc::reconstruct(BoPtr());
	BoCell.write();*/

        if (writeResults)
        {
            BoPtr().write();

	volVectorField cellBo
            (
                IOobject
                (
                    "cellBo",
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ
                ),
                fvc::reconstruct(BoPtr()*mesh.magSf())
            );

            cellBo.write();
        }
    }
    else
    {
        Info<< "    No phi" << endl;
    }

    Info<< "\nEnd\n" << endl;
}


// ************************************************************************* //
