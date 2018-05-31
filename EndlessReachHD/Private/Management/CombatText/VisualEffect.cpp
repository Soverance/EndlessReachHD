// © 2014 - 2017 Soverance Studios
// http://www.soverance.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "EndlessReachHD.h"
#include "VisualEffect.h"

// Sets default values
AVisualEffect::AVisualEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates a scene component and sets it as the root
	Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	RootComponent = Root;

	StatusFX = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("StatusFX"));
	StatusFX->SetupAttachment(Root);
	StatusFX->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AVisualEffect::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVisualEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AVisualEffect::StartFX(FCombatEffectStruct Effect)
{
	CombatEffect = Effect;
	StatusFX->SetTemplate(CombatEffect.Effect);
	StatusFX->Activate();
}

