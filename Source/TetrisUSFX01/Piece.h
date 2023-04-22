// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include <functional>
#include "Piece.generated.h"

using namespace std;

UCLASS()
class TETRISUSFX01_API APiece : public AActor {

	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APiece();

protected:
	// inicializamos al metodo begin play -> se ejecuta al iniciar el juego
	virtual void BeginPlay() override;

	// inicializamos al metodo end play -> se ejecuta al finalizar el juego
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// inicalizamos el metodo tick -> se ejecuta cada frame del juego
	virtual void Tick(float DeltaTime) override;

	//creamos la clase SceneComponent -> es un componente que nos permite mover el actor
	UPROPERTY()
		class USceneComponent* SceneComponent;

	//creamos los metodos para interactuar con la pieza
	void DrawDebugPiece();
	void TestRotate();

	void TestRotateFB();

	void MoveLeft();
	void MoveRight();

	void MoveDownGraduality();

	bool MoveDown(bool PlaySound = true);


	class USoundCue* RotateSoundCue;

	class USoundCue* MoveLeftRightSoundCue;

	void Dismiss();
	bool CheckWillCollision(function<FVector(FVector OldLocation)> ChangeBeforeCheck);

	void SpawnPieces();
	
	//creamos un array para los colores de los bloques
		TArray<class UMaterial*> Colors;

	float directionP = 0;
private:
	// creamos un array para los bloques de la pieza creada
	TArray<ABlock*> Blocks;
};
