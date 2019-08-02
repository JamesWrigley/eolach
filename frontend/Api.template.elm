module Api exposing (Book, getBooks)

import Http
import Url.Builder as UB
import Json.Decode exposing (..)


{- Types -}

type alias Book =
    { title : String,
      author : String,
      subjects : List String,
      section : String
    }

{- API methods -}

getBooks : (Result Http.Error (List Book) -> msg) -> Cmd msg
getBooks handler =
    let
        attributes = "title,author,subjects,section"
    in Http.get
        { url = getEndpointBase "books" [("attributes", attributes)],
          expect = Http.expectJson handler bookListDecoder
        }

{- Decoders -}

bookListDecoder : Decoder (List Book)
bookListDecoder =
    list bookDecoder

bookDecoder : Decoder Book
bookDecoder =
    map4 Book
        (field "title" string)
        (field "author" string)
        (field "subjects" (list string))
        (field "section" string)

{- Helper functions/variables -}

getEndpointBase : String -> List (String, String) -> String
getEndpointBase endpoint queryStrings =
    UB.crossOrigin
        url [ endpoint ]
            (List.map (\(name, value) -> UB.string name value) queryStrings)

getEndpoint : String -> String
getEndpoint endpoint =
    getEndpointBase endpoint []

-- This gets substituted with a terraform output during deployment
url : String
url = "API_URL"
