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

	//creamos los metos para interactuar con la pieza
	void DrawDebugPiece();
	void TestRotate();
	void MoveLeft();
	void MoveRight();
	bool MoveDown(bool PlaySound = true);
	void Dismiss();
	bool CheckWillCollision(function<FVector(FVector OldLocation)> ChangeBeforeCheck);

	void SpawnPieces();
	//creamos un array para los colores de los bloques
	UPROPERTY(EditAnywhere)
		TArray<class UMaterial*> Colors;

private:
	// creamos un array para los bloques de la pieza creada
	TArray<ABlock*> Blocks;
};
