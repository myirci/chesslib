#include "BoardBase.hpp"

#include <ChessLib/Chess/Utility.hpp>

#include <stdexcept>

namespace chesslib
{

#pragma region BoardBase_Methods

	BoardBase::BoardBase() :
		_active_color{ 0 },
		_castling_rights{ 0 },
		_enpassant_target{ squareset::None },
		_halfmove_clock{ 0 },
		_fullmove_clock{ 1 }	
	{ }

	Color BoardBase::GetActiveColor() const { return _active_color; }

	Square BoardBase::GetEnPassantSquare() const { return _enpassant_target; };

	uint16_t BoardBase::GetHalfMoveClock() const { return _halfmove_clock; };

	uint16_t BoardBase::GetFullMoveClock() const { return _fullmove_clock; };

	bool BoardBase::IsCastlingAvailable() const { return _castling_rights != 0; }

	bool BoardBase::QueryCastling(Castling c) const { return _castling_rights & static_cast<int8_t>(c); }

	void BoardBase::SetCastling(Castling c, bool flag)
	{
		_castling_rights = flag
			? _castling_rights | static_cast<int8_t>(c)
			: _castling_rights & ~static_cast<int8_t>(c);
	}

	void BoardBase::SetActiveColor(char side_to_move) 
	{
		_active_color = side_to_move == charset::White ? color::White : color::Black;
	}

	void BoardBase::SetCastlingRights(std::string_view castling_availability)
	{
		if (castling_availability != "-")
		{
			for (char c : castling_availability)
			{
				switch (c)
				{
					case charset::WhiteKing: SetCastling(Castling::WHITE_KS, true); break;
					case charset::WhiteQueen: SetCastling(Castling::WHITE_QS, true); break;
					case charset::BlackKing: SetCastling(Castling::BLACK_KS, true); break;
					case charset::BlackQueen: SetCastling(Castling::BLACK_QS, true); break;
					default:
						throw std::logic_error("Fen parse error - invalid castling rights.");
				}
			}
		}
	}

	void BoardBase::SetEnPassantSquare(Square ep) { _enpassant_target = ep; }

	void BoardBase::SetHalfMoveClock(std::string_view hmc) 
	{
		auto h = utility::numeric::to_int(hmc);
		if (!h.has_value())
			throw std::logic_error("Fen parse error - invalid half move clock.");
		_halfmove_clock = static_cast<uint16_t>(h.value());
	}

	void BoardBase::SetFullMoveClock(std::string_view fmc) 
	{
		auto f = utility::numeric::to_int(fmc);
		if (!f.has_value())
			throw std::logic_error("Fen parse error - invalid full move clock.");
		_fullmove_clock = static_cast<uint16_t>(f.value());
	}

	const BoardBase::PinMap& BoardBase::GetPins() const { return _pins; }

	const BoardBase::CheckList& BoardBase::GetChecks() const { return _checks; }

	Direction BoardBase::GetPinDirection(Square piece_loc) const 
	{
		auto it = _pins.find(piece_loc);
		return it == _pins.end() ? direction::None : it->second.second;
	}

#pragma endregion

#pragma region BoardBaseWithPieces_Methods

	// Explicit initialization
	template Square BoardBaseWithPieces::GetKingPosition<color::White>() const;
	template Square BoardBaseWithPieces::GetKingPosition<color::Black>() const;
	template BoardBaseWithPieces::EqualPieceRange BoardBaseWithPieces::GetPiecePositions<color::White>(Piece) const;
	template BoardBaseWithPieces::EqualPieceRange BoardBaseWithPieces::GetPiecePositions<color::Black>(Piece) const;
	
	BoardBaseWithPieces::BoardBaseWithPieces() { }

	const BoardBaseWithPieces::PieceMap& BoardBaseWithPieces::GetWhitePieces() const { return _white_pieces; }

	BoardBaseWithPieces::PieceMap& BoardBaseWithPieces::GetWhitePieces() { return _white_pieces; }

	const BoardBaseWithPieces::PieceMap& BoardBaseWithPieces::GetBlackPieces() const { return _black_pieces; }

	BoardBaseWithPieces::PieceMap& BoardBaseWithPieces::GetBlackPieces() { return _black_pieces; }

	template <Color Clr>
	Square BoardBaseWithPieces::GetKingPosition() const 
	{
		if constexpr (Clr == color::White) 
		{
			auto it = _white_pieces.find(pieceset::WhiteKing);
			if (it == _white_pieces.end())
				throw std::logic_error("White king could not be found");
			return it->second;
		}
		else 
		{
			auto it = _black_pieces.find(pieceset::BlackKing);
			if (it == _black_pieces.end())
				throw std::logic_error("Black king could not be found");
			return it->second;
		}
	}

	template <Color Clr>
	BoardBaseWithPieces::EqualPieceRange BoardBaseWithPieces::GetPiecePositions(Piece p) const
	{
		if constexpr (Clr == color::White)
			return _white_pieces.equal_range(p);
		else 
			return _black_pieces.equal_range(p);
	}
#pragma endregion

}