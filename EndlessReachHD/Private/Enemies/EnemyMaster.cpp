// © 2012 - 2019 Soverance Studios
// https://soverance.com

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
#include "Kismet/KismetMathLibrary.h"
#include "Management/AI/EnemyAI.h"
#include "EnemyMaster.h"

// Sets default values
AEnemyMaster::AEnemyMaster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // sets capsule to ignore camera collision
	GetMesh()->bReceivesDecals = false;  // make this mesh ignore all decals
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // make this mesh ignore collision
	GetMesh()->bCastCapsuleIndirectShadow = true;  // make this mesh cast physics based capsule shadows

	// Create Combat Text Component
	CombatTextComponent = CreateDefaultSubobject<UCombatTextComponent>(TEXT("Combat Text Component"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeObject(TEXT("BehaviorTree'/Game/Enemies/EnemyBT.EnemyBT'"));
	EnemyBehavior = BehaviorTreeObject.Object; // Set Behavior Tree
	AIControllerClass = AEnemyAI::StaticClass();  // Set AI Class
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bIsDead = false;
	bIsHit = false;
	bIsTargetable = true;
	bIsAttacking = false;
	bInRange = false;
	bIsMoving = false;
	bIsAggroed = false;
}

// Called when the game starts or when spawned
void AEnemyMaster::BeginPlay()
{
	Super::BeginPlay();

	PawnSensing->OnHearNoise.AddDynamic(this, &AEnemyMaster::OnHearNoise);  // bind the OnHearNoise event
	PawnSensing->OnSeePawn.AddDynamic(this, &AEnemyMaster::OnSeePawn);  // bind the OnSeePawn event
}

// Called every frame
void AEnemyMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsDead) // if enemy is not dead
	{
		if (bInRange)  // if is in range of player
		{
			if (bIsAggroed)  // if is aggroed to the player
			{
				//LookAtTarget();  // always face the player when chasing!

				if (GetDistanceTo(Target) > PawnSensing->SightRadius)  // if the player gets out of the enemy's sight radius while chasing...
				{
					// Lost sight of player, disable aggro
					//bInRange = false;
					//Deaggro();
				}
			}
		}
	}
}

// Show combat damage text function
void AEnemyMaster::ShowCombatDamageText(bool IsCritical, float Damage)
{
	if (IsCritical)
	{
		// CRITICAL DAMAGE
		FString String = FString::SanitizeFloat(Damage);  // Get string from Damage value
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_CritDmg, FText::FromString(String));  // show combat text
	}
	else
	{
		// STANDARD DAMAGE
		FString String = FString::SanitizeFloat(Damage);  // Get string from Damage value
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Damage, FText::FromString(String));  // show combat text
	}
}

// Sets the Enemy's default stats
void AEnemyMaster::SetBaseStats()
{
	float HPMod = 0;
	float StatMod = 0.0f;

	if (BattleType == EBattleTypes::BT_Bounty) // High stats for bounties
	{
		HPMod = 3500;
		StatMod = 10.0f;
	}
	if (BattleType == EBattleTypes::BT_Boss) // High stats for bosses
	{
		HPMod = 2000;
		StatMod = 7.5f;
	}
	if (BattleType == EBattleTypes::BT_Standard) // lower stats for regular enemies
	{
		HPMod = 200;
		StatMod = 5.1f;
	}

	// SET ALL STATS BASED ON CURRENT LEVEL + MODIFIERS
	HP_Current = Level * HPMod;
	HP_Max = Level * HPMod;
	// the randoms are just to add slight variations to the enemy's stats.
	ATK = (Level * StatMod) + FMath::RandRange(-3.0f, 3.0f);
	DEF = (Level * StatMod) + FMath::RandRange(-3.0f, 3.0f);
}

// Force HP Caps keeps the enemy's HP between 0 and Max
bool AEnemyMaster::ForceHPCaps()
{
	bool Kill = false;

	if (HP_Current > HP_Max)  // if HP is greater than 0
	{
		HP_Current = HP_Max;
	}
	else if (HP_Current < 0)  // if HP is less than 0
	{
		HP_Current = 0;
		Kill = true;
	}

	if (Kill)
	{
		Death(); // Start the Death sequence
	}

	return Kill;
}

// Calculates the damage dealt by the enemy this turn. Must pass in the base ATK value of the specific attack.
float AEnemyMaster::EnemyDealDamage(float BaseATK)
{
	float atk1 = (ATK + BaseATK) / 32;
	float atk2 = (ATK * BaseATK) / 32;
	float atk3 = atk1 * atk2;
	float atk4 = (atk3 + ATK) * 40;
	DamageOutput = atk4;

	return DamageOutput;
}

// Calculates the damage taken by a player attack
void AEnemyMaster::EnemyTakeDamage(float DamageTaken)
{
	bIsHit = true; // Enemy was hit - setting this to true will trigger a hit animation
	HitFX->Activate();  // Activate the HitFX particle

	float critical = FMath::FRandRange(1.0f, 1.5f);
	float mod1 = FMath::Abs(512 - DEF);  // Take the Absolute Value of 512 - DEF
	float mod2 = critical * mod1 * DamageTaken;
	float mod3 = FMath::FloorToInt(mod2 / (16 * 512));  // calculate final damage taken
	HP_Current = HP_Current - mod3;  // Finally remove the damage taken

	if (critical > 1.4f)
	{
		// Display Critical Damage
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_CritDmg, UCommonLibrary::GetFloatAsTextWithPrecision(mod3, 0, false));
	}
	else
	{
		// Display Normal Damage
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Damage, UCommonLibrary::GetFloatAsTextWithPrecision(mod3, 0, false));
	}

	ForceHPCaps(); // force HP caps
}

// Sets RunAI to true, allowing the enemy's behavior tree to begin running
void AEnemyMaster::RunToTarget()
{
	if (!bIsDead)
	{
		if (!Target->bIsDead)
		{
			bIsAggroed = true;
			bIsAttacking = false;
			bRunAI = true;  // Allow AI to be running
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Homing in on target!")));
		}
	}
}

// Makes the enemy always face the target it's chasing
void AEnemyMaster::LookAtTarget()
{
	if (!bIsDead)
	{
		if (!Target->bIsDead)
		{
			FVector EnemyLocation = GetActorLocation(); // Get actor rotation
			FVector PlayerLocation = Target->GetActorLocation();  // get player location
			FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);  // find look at rotation
			FRotator CorrectedRot = FRotator(LookAtRot.Pitch, LookAtRot.Yaw, LookAtRot.Roll);  // correct rotation
			SetActorRotation(CorrectedRot); // set rotation to the corrected rotation
		}
	}
}

// AGGRO
void AEnemyMaster::Aggro(APawn* Pawn)
{
	if (!bIsDead) // make sure this enemy is not dead before doing anything
	{
		AEndlessReachHDPawn* Player = Cast<AEndlessReachHDPawn>(Pawn); // Check if the pawn seen by enemy AI was indeed the player
		if (Player)
		{
			// Make sure the enemy actually has Line of Sight to the player
			if (Controller->LineOfSightTo(Player, FVector(0, 0, 0), false))
			{
				if (!Player->bIsDead)  // Make sure the player isn't dead before going further
				{
					Target = Player; // Set the seen player as the new Target
					bInRange = true; // the enemy is now in range
					bIsAggroed = true; // the enemy is now aggroed
					bIsTargetable = true;  // turn on targeting, in case it was previously disabled
					OnAggro.Broadcast();  // call custom enemy aggro function (usually the enemy specific attack cycle)

					// IF STANDARD ENEMY
					if (BattleType == EBattleTypes::BT_Standard)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Enemy Aggro!")));
					}
					// IF BOSS ENEMY
					if (BattleType == EBattleTypes::BT_Boss)
					{

					}
					// IF BOUNTY ENEMY
					if (BattleType == EBattleTypes::BT_Bounty)
					{

					}
				}
			}
		}
	}
}

// DEAGGRO
void AEnemyMaster::Deaggro()
{
	// Standard enemies can be deaggroed by escaping their AI sensing range
	if (BattleType == EBattleTypes::BT_Standard)
	{
		bIsAggroed = false;
	}
	// Boss and Signet enemies cannot be deaggroed in the traditional manner.
	if (BattleType == EBattleTypes::BT_Boss || BattleType == EBattleTypes::BT_Bounty)
	{
		if (bIsDead)
		{
			bIsAggroed = false;
		}
		if (!bIsDead)
		{
			if (Target)
			{
				// boss enemies only deaggro in this manner if the player is dead. i.e., you cannot run from bosses.
				if (Target->bIsDead)
				{
					bIsAggroed = false;
				}
			}
		}
	}
}

// End All Attack Rounds
void AEnemyMaster::EndAttackRound()
{
	bIsAttacking = false;
}

// DEATH
void AEnemyMaster::Death()
{
	bIsDead = true;
	bRunAI = false;
	bIsTargetable = false;  // turn off targeting if dead.
	GetMovementComponent()->StopMovementImmediately();  // Stop Movement
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Deaggro();  // Deaggro
	OnDeath.Broadcast();  // broadcast the OnDeath event dispatcher, which will run enemy specific death code like animations, effects, and rewards
}

// FINAL DEATH
void AEnemyMaster::FinalDeath(bool ShouldDestroy)
{
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f); // normal motion
	DeathFX->Activate();
	// Do the Disappear effect after a short delay
	FTimerHandle DisappearTimer;
	GetWorldTimerManager().SetTimer(DisappearTimer, this, &AEnemyMaster::Disappear, 1.5f, false);
	
	if (ShouldDestroy)
	{
		// Destroy the enemy after a short delay if this function was called with ShouldDestroy set to true
		FTimerHandle DestroyTimer;
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemyMaster::DestroyEnemy, 5.5f, false);
	}
}

void AEnemyMaster::Disappear()
{
	GetMesh()->SetVisibility(false);  // Hide the enemy mesh
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//Deaggro(); // Run Deaggro code
}

void AEnemyMaster::DestroyEnemy()
{
	Destroy(); // Destroy this actor
}

void AEnemyMaster::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if (!bIsDead)
	{
		if (!bIsAggroed)
		{
			Aggro(PawnInstigator);
			RunToTarget();
		}
	}
}

void AEnemyMaster::OnSeePawn(APawn* PawnInstigator)
{
	if (!bIsDead)
	{
		if (!bIsAggroed)
		{
			Aggro(PawnInstigator);
			RunToTarget();
		}
	}
}