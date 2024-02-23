// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpGameMode.h"
#include <fstream>

AHttpGameMode::AHttpGameMode()
{
    //Boolean variables for emotions
    IsHappy = false;
    IsDancing = false;
    IsSad = false;
    IsAngry = false;
    IsThinking = false;
    IsGreeting = false;

    //Adding words to the set that will trigger the animations
    HappyKeywords.Add("happy");
    HappyKeywords.Add("excited");
    HappyKeywords.Add("joy");
    HappyKeywords.Add("great");
    HappyKeywords.Add("like");
    HappyKeywords.Add("love");

    SadKeywords.Add("sad");
    SadKeywords.Add("gross");
    SadKeywords.Add("horrible");
    SadKeywords.Add("unpleasent");
    SadKeywords.Add("sorry");
    SadKeywords.Add("uncomfortable");

    AngryKeywords.Add("angry");
    AngryKeywords.Add("frustrated");
    AngryKeywords.Add("ashamed");
    AngryKeywords.Add("anger");

    ThinkKeywords.Add("think");
    ThinkKeywords.Add("however");
    ThinkKeywords.Add("understand");
    ThinkKeywords.Add("neutral");
    ThinkKeywords.Add("analyze");
    ThinkKeywords.Add("depend");

    DanceKeywords.Add("dance");
    DanceKeywords.Add("groove");
    DanceKeywords.Add("moves");

    GreetKeywords.Add("hello!");
    GreetKeywords.Add("hello there!");
    GreetKeywords.Add("hi!");   
    GreetKeywords.Add("hi there!");
    GreetKeywords.Add("hey there!");    
    GreetKeywords.Add("welcome");
}


//Getter and Setter for questionToAsk
void AHttpGameMode::SetQuestionToAsk(FString NewQuestion)
{
    questionToAsk = NewQuestion;
}

FString AHttpGameMode::GetQuestionToAsk()
{
    return questionToAsk;
}

//Getter and Setter for APIResponse
FString AHttpGameMode::GetAPIResponse()
{
    return APIResponse;
}

void AHttpGameMode::SetAPIResponse(FString NewAnswer) {

    APIResponse = NewAnswer;
}


//Setting the value of bools to trigger related animations inside blueprint.
void AHttpGameMode::SetIsHappy(bool situation)
{
    IsHappy = situation;
}

void AHttpGameMode::SetIsDancing(bool situation)
{
    IsDancing = situation;
}

void AHttpGameMode::SetIsGreeting(bool situation)
{
    IsGreeting = situation;
}

void AHttpGameMode::SetIsSad(bool situation)
{
    IsSad = situation;
}

void AHttpGameMode::SetIsAngry(bool situation)
{
    IsAngry = situation;
}

void AHttpGameMode::SetIsThinking(bool situation)
{
    IsThinking = situation;
}

void AHttpGameMode::StartPlay()
{
    Super::StartPlay();
}

//Setting the boolean variables after checking APIResponse that if there is the word for the related animation. If there is set it true
void AHttpGameMode::ProcessResponseText(FString ResponseText)
{
    FString negative[] = {"don't","can't","cannot","do not", "not able"};
    bool negativeFound = false;

    for (const FString& negativeWords : negative) {
        if (ResponseText.Contains(negativeWords)) {
            negativeFound = true;
        }
    }
    IsHappy = CheckWords(ResponseText, HappyKeywords);
    IsSad = CheckWords(ResponseText, SadKeywords);
    IsAngry = CheckWords(ResponseText, AngryKeywords);
    IsDancing = CheckWords(ResponseText, DanceKeywords) && !negativeFound;
    IsThinking = CheckWords(ResponseText, ThinkKeywords);
    IsGreeting = CheckWords(ResponseText, GreetKeywords);

}

//With ResponseText and KeywordSet variable, check if it contains any word from the set and return the value
bool AHttpGameMode::CheckWords(FString& ResponseText, TSet<FString>& KeywordSet)
{
    for (const FString& Keyword : KeywordSet) {
        if (ResponseText.Contains(Keyword)) {
            return true;
        }
    }
    return false;
}

//Creating request of Bard API
void AHttpGameMode::MakeHTTPRequest(FString NewQuestion)
{
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

    TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
    TSharedRef<FJsonObject> PromptObj = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> MessagesArray;

    SetQuestionToAsk(NewQuestion);

    TSharedRef<FJsonObject> MessageObj = MakeShared<FJsonObject>();
    MessageObj->SetStringField("content", GetQuestionToAsk());
    MessagesArray.Add(MakeShared<FJsonValueObject>(MessageObj));

    PromptObj->SetStringField("context", context);
    MessageObj->SetStringField("author", "0");
    PromptObj->SetArrayField("messages", MessagesArray);

    RequestObj->SetObjectField("prompt", PromptObj);
    RequestObj->SetNumberField("temperature", 0.5);
    RequestObj->SetNumberField("top_k", 40);
    RequestObj->SetNumberField("top_p", 0.95);
    RequestObj->SetNumberField("candidate_count", 1);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObj, Writer);

    Request->OnProcessRequestComplete().BindUObject(this, &AHttpGameMode::OnResponseReceived);
    Request->SetURL(urlRequest);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetContentAsString(RequestBody);
    Request->ProcessRequest();
}

//Getting response of Bard API
void AHttpGameMode::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
    if (bConnectedSuccessfully && Response.IsValid())
    {
        const FString ResponseBody = Response->GetContentAsString();

        TSharedPtr<FJsonObject> ResponseObj;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

        if (FJsonSerializer::Deserialize(Reader, ResponseObj))
        {
            const TArray<TSharedPtr<FJsonValue>> CandidatesArray = ResponseObj->GetArrayField("candidates");
            TSharedPtr<FJsonObject> CandidateObj = CandidatesArray[0]->AsObject();

            FString Text = CandidateObj->GetStringField("content");
            SetAPIResponse(Text);
            ProcessResponseText(GetAPIResponse());
        }
    }             
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to connect to the API"));
    }
}

//Creating request of ElevenLabs API
/*void AHttpGameMode::VoiceAPIRequest()
{
    FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
    TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
    TSharedRef<FJsonObject> VoiceSettings = MakeShared<FJsonObject>();

    //Adding voice settings
    VoiceSettings->SetNumberField("stability", 0.5);
    VoiceSettings->SetNumberField("similarity_boost", 0.5);

    RequestObj->SetStringField("model_id", "eleven_multilingual_v2");
    RequestObj->SetStringField("text", GetAPIResponse());
    RequestObj->SetObjectField("voice_settings", VoiceSettings);

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObj, Writer);

    Request->OnProcessRequestComplete().BindUObject(this, &AHttpGameMode::OnVoiceReceived);
    Request->SetURL("https://api.elevenlabs.io/v1/text-to-speech/EXAVITQu4vr4xnSDxMaL/stream?optimize_streaming_latency=4");
    Request->SetVerb("POST");
    Request->SetHeader("Accept", "audio/mpeg");
    Request->SetHeader("Content-Type", "application/json");
    Request->SetHeader("xi-api-key", "50647fc9383ed7d947b36ba86eff7651");
    Request->SetContentAsString(RequestBody);

    UE_LOG(LogTemp, Warning, TEXT("Request Payload: %s"), *RequestBody);
    Request->ProcessRequest();

}

//Getting response of ElevenLabs API
void AHttpGameMode::OnVoiceReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool isSuccessfull)
{
    if (isSuccessfull && Response.IsValid()) {

        FString ContentType = Response->GetHeader("Content-Type");
        UE_LOG(LogTemp, Warning, TEXT("Response Content-Type: %s"), *ContentType);
        ResponseContent = Response->GetContent();

   
    }
}*/