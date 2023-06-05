// Copyright Epic Games, Inc. All Rights Reserved.


#include "TetrisUSFX01GameModeBase.h"

#include "BluePiece.h"
#include "ArchitecturalEngineer.h"
#include "BPiece.h"

#include "Board.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

ATetrisUSFX01GameModeBase::ATetrisUSFX01GameModeBase() {
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = ABoard::StaticClass();
    siguientePosicionZ = 0;
}

void ATetrisUSFX01GameModeBase::BeginPlay()
{
    // Call the parent class version of this function
    Super::BeginPlay();

    for (TActorIterator<ACameraActor> it(GetWorld()); it; ++it)
    {
        UE_LOG(LogTemp, Warning, TEXT("Find camera: %s"), *(it->GetFName().ToString()));
        if (it->GetFName() == TEXT("BoardCamera"))
        {
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            PC->SetViewTargetWithBlend(*it, 0.5);
            break;
        }
    }

    //patron builder
    // 
    //Spawn Builder and Engineer
    BluePieceBuilder = GetWorld ()->SpawnActor<ABluePiece>(ABluePiece::StaticClass ());
    Engineer = GetWorld ()->SpawnActor<AArchitecturalEngineer>(AArchitecturalEngineer::StaticClass ());

    //Set the Builder for the Engineer and create the buildings
    Engineer->SetPieceBuilder (BluePieceBuilder);
    Engineer->ConstructPiece ();

    //Get the Engineer's Lodging and Logs the created buildings
    ABPiece* Block = Engineer->GetPiece();
    Block->PieceCharacteristics();

}

// Called every frame
void ATetrisUSFX01GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void ATetrisUSFX01GameModeBase::DestroyActorFunction()
{
	if (SpawnedActor != nullptr)
   {
      // Displays a red message on the screen for 10 seconds
      GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("Actor Block destruido"));
      SpawnedActor->Destroy();
   }
}
