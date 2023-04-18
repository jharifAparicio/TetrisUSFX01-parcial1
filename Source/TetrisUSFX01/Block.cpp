// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

// constructor
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//creamos un objeto de tipo StaticMeshComponent y lo ruteamos a la raiz del actor
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	RootComponent = BlockMesh;

	//cargamos el la malla o textura del bloque
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BlockMeshAsset(TEXT("StaticMesh'/Game/Mesh/block.block'"));

	//si la malla o textura existe, la asignamos al objeto
	if (BlockMeshAsset.Succeeded()){
		BlockMesh->SetStaticMesh(BlockMeshAsset.Object);
		BlockMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("non-existent mesh for the block"));
	}
}

// metodo begin play -> solo se ejecuta una vez
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// metodo tick -> se ejecuta cada frame o cada cierto tiempo
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

