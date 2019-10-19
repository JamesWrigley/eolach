module Api exposing (Book, getBooks)

import Http
import Time
import Url.Builder as UB
import Json.Decode exposing (..)


{- EOLACH QUERY PATTERNS

Item filters can be any combination of books, DVD's, and CD's.

For items:
 * List all items, taking account of any item filters applied
 * Search items by title, author/speaker, and subject, taking account of item filters
 * List the history of an item

For patrons:
 * List all patrons
 * Search patrons by name
 * List the history of a patron
-}


{- Types -}

type alias Item item =
  { item |
      title : String,
      subjects : List String,
      code : String
  }

type alias Book =
    { title : String,
      author : String,
      subjects : List String,
      code : String
    }

type alias Dvd =
  { title : String,
    subjects: List String,
    code : String
  }

type alias TeachingDvd =
  { title : String,
    speaker : String,
    subjects: List String,
    code : String
  }

type alias Cd =
  { title : String,
    speaker : String,
    subjects : List String,
    code : String
  }

type alias Patron =
  { name : String,
    number : Maybe String,
    email : Maybe String
  }

type alias Loan =
  { patronId : String,
    itemId : String,
    date : Time.Posix
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
        (field "code" string)

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
