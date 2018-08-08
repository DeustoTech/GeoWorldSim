#include "EatBehaviour.h"
#include "../../usecase/tesis/SheepAgent.h"


EatBehaviour::EatBehaviour(GWSAgent *behaving_agent) : GWSBehaviour( behaving_agent ){
}

bool EatBehaviour::finished(){
    return false;
}

bool EatBehaviour::behave(){



    // Check your energy:
    if (this->getAgent()->getProperty("energy").toDouble() < 1.)
        {
        qInfo() << "RIP" << this->getAgent()->getProperty("@id").toString();
        this->getAgent()->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
        //QTimer::singleShot( 0 , this->getAgent() , &GWSAgent::deleteLater );
        return true;
        }
    else
        {

        qInfo() << "              " <<  this->getAgent()->getProperty("@type").toString() << this->getAgent()->getProperty("@id").toString();
        qInfo() << "               -------------------                           ";

        /* Move randomly through random index generator */
        int direction[3] = {0, 1, -1}; // the possible displacements of going NORTH, SOUTH, EAST or WEST
        int RandIndexX = qrand() % 3; //generates a random number between 0 and 2
        int RandIndexY = qrand() % 3; //generates a random number between 0 and 2


        /* Move coordinates according to random index */
        int TargetX = direction[RandIndexX];
        int TargetY = direction[RandIndexY];


        /* We assume an GWSAgent will only nourish/breed when it moves to a different position. */
        qInfo() << "Target movement = ("<< TargetX << "," << TargetY<< ")";


        if ((TargetX == 0) && (TargetY == 0)) // Sometimes the Agent will choose to stay on same position
           {
           qInfo() << "You choose to stay at the same position. You will eventually die of starvation.";
           this->getAgent()->setProperty("energy", this->getAgent()->getProperty("energy").toDouble() * 0.6);
           }
        else // And sometimes they will choose to move
           {
            // Move
            GWSPhysicalEnvironment::globalInstance()->transformMove( this->getAgent() , GWSCoordinate( TargetX, TargetY ) );

            //Moving consumes energy
            double initialEnergy = this->getAgent()->getProperty("energy").toDouble();
            double moveLosses = initialEnergy * 0.25;

            //Final energy of Agent*/
            double finalEnergy = initialEnergy - moveLosses;
            this->getAgent()-> setProperty("energy", finalEnergy);

            // And see what is around you:
            QList<GWSAgent*> CellOccupation = dynamic_cast<ViewSkill*>( this->getAgent()->getSkill( ViewSkill::staticMetaObject.className() ) )->getViewingAgents();
            qInfo() << "Cell Occupation = " << CellOccupation;

            int Occupation = 0;

            // We need to differentiate between preys: wolf kills sheep but sheep just reduces grass' energy:
            if (CellOccupation.size() == 0)
                {
                qDebug() << "Nothing on this cell!";
                return true;
                }
            else
                {
                for (int i = 0; i < CellOccupation.size(); i++)
                    {
                    if (CellOccupation.at(i)->getProperty("@type") == this->getProperty("prey"))
                        {
                        if (this->getProperty("prey").toString() == "PastureAgent")
                            {
                            // Eat
                            qDebug() << "Found pasture!";
                            double foodGains = 25.;
                            double finalEnergy = this->getAgent()->getProperty("energy").toDouble() + foodGains;
                            this->getAgent()-> setProperty("energy", finalEnergy);

                            // Less grass = less energy of PastureAgent
                            CellOccupation.at(i)->setProperty( "energy", CellOccupation.at(i)->getProperty("energy").toDouble() * 0.9 );
                            //return true;
                            }
                        if (this->getProperty("prey").toString() == "SheepAgent")
                            {
                            // Eat
                            qDebug() << "Found sheep!";
                            double foodGains = 50.;
                            double finalEnergy = this->getAgent()->getProperty("energy").toDouble() + foodGains;
                            this-> setProperty("energy", finalEnergy);

                            qInfo() << "RIP" << CellOccupation.at(i)->getProperty("@id").toString();
                            CellOccupation.at(i)->setProperty("energy" , 0.);
                            CellOccupation.at(i)->icon_url = "https://image.flaticon.com/icons/svg/236/236322.svg";
                            //QTimer::singleShot( 0 , CellOccupation.at(i) , &GWSAgent::deleteLater );
                            //return true;
                            }
                        }
                    if (CellOccupation.at(i)->getProperty("@type") == this->getAgent()->getProperty("@type"))
                       {
                       Occupation +=1;
                       }

                    }

                qInfo() << "Congeners in your cell = " << Occupation;

                if (Occupation >= 1)
                    {
                    //Breed
                    qInfo() << "Found mate! ";

                    //Breeding consumes energy
                    this->getAgent()->setProperty("energy" , this->getAgent()->getProperty("energy").toDouble() * 0.5);

                    //Add offspring to the World
                    QJsonObject this_json = this->getAgent()->serialize();
                    this_json.insert( GWS_ID_PROP , QJsonValue::Undefined );
                    GWSAgent* OffspringAgent = dynamic_cast<GWSAgent*>( GWSObjectFactory::globalInstance()->fromJSON( this_json ) );
                    GWSExecutionEnvironment::globalInstance()->registerAgent( OffspringAgent );
                    qInfo() << "OffspringAgent's initial position = (" << GWSPhysicalEnvironment::globalInstance()->getGeometry( OffspringAgent )->getCentroid().getX() << "," << GWSPhysicalEnvironment::globalInstance()->getGeometry( OffspringAgent )->getCentroid().getY() << ")";
                    }

                }

            }


        }



}
