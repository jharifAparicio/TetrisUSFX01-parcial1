// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Block.h"
#include "Piece.h"
#include "TetrisUSFX01GameModeBase.generated.h"

UCLASS()
class TETRISUSFX01_API ATetrisUSFX01GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATetrisUSFX01GameModeBase ();

private:
	class ABluePiece *BluePieceBuilder;
	class AArchitecturalEngineer *Engineer;

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY()
	ABlock* SpawnedActor;
	
	UPROPERTY()
	APiece* SpawnedPiece;

	UFUNCTION()
	void DestroyActorFunction();

	virtual void Tick(float DeltaTime) override;
private:
	float siguientePosicionZ;
};
