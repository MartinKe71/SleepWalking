#include "FontSource.hpp"
#include <iostream>


FontSource::FontSource(const std::string font, const unsigned int s) 
{
	font_size = s;

	FT_Error ft_error;
	if ((ft_error = FT_Init_FreeType(&ft_library)))
	{
		std::cerr << "FT_Library initialization failed." << std::endl;
		abort();
	}

	if ((ft_error = FT_New_Face(ft_library, font.c_str(), 0, &ft_face)))
	{
		std::cerr << "FT_Face creation failed." << std::endl;
		abort();
	}

	if ((ft_error = FT_Set_Char_Size(ft_face, font_size * 64, font_size * 64, 0, 0)))
	{
		std::cerr << "FT set size failed." << std::endl;
		abort();
	}

	if (ft_error == FT_Err_Unknown_File_Format)
	{
		std::cerr << "Font file not supported." << std::endl;
		abort();
	}

	InitializeGlyaphMap();
}

FontSource::~FontSource() {
	ClearText();
	ClearTextureMap();
}

void FontSource::InitializeGlyaphMap() 
{
	for (unsigned char c = 32; c <= 127; c++)
	{
		FT_Error error = FT_Load_Char(ft_face, c, FT_LOAD_RENDER);
		if (error)
		{
			std::cerr << "Failed to load " << c << std::endl;
			continue;
		}

		auto& bit_map = ft_face->glyph->bitmap;

		GLuint texture_id = shape_texture_program->GetTextureId(bit_map);

		glyph_map.insert(std::pair<char, GLuint>(c,texture_id));
	}
}

void FontSource::DrawText(const glm::uvec2& drawable_size, const std::string& text, glm::vec2 anchor, glm::vec2 dims, glm::u8vec4 color)
{
	SetText(text);

	float x_span = dims.x * drawable_size.x / 2.0f;
	//std::cout << "\n\nx span: " << x_span << "\n";

	float x_start = AnchorToScreen(anchor.x, drawable_size.x);
	float y_start = AnchorToScreen(anchor.y, drawable_size.y);
	y_start -= font_size;

	float x_origin = x_start;
	float y_origin = y_start;

	FT_GlyphSlot slot = ft_face->glyph;
	auto& bitmap = slot->bitmap;
	//std::cout << "\n\n-----------------------\n";
	//std::cout << "start: x:" << x_start << "; y: " << y_start << "\n";

	for (size_t i = 0; i < text.length(); i++) {
		auto x_offset = glyph_pos[i].x_offset / 64.0f;
		auto y_offset = glyph_pos[i].y_offset / 64.0f;
		auto x_advance = glyph_pos[i].x_advance / 64.0f;
		auto y_advance = glyph_pos[i].y_advance / 64.0f;

		// Horizontal Text Wrapping
		if (x_start + x_advance - x_origin > x_span || text[i] == '\n') {
			x_start = x_origin;
			y_origin -= font_size * 1.25f;
			y_start = y_origin;
			if (text[i] == '\n' || text[i] == ' ')
				continue;
		}

		FT_Error error = FT_Load_Char(ft_face, text[i], FT_LOAD_RENDER);
		if (error)
			continue;

		float char_start_x = x_start + x_offset + slot->bitmap_left;
		char_start_x = ScreenToAnchor(char_start_x, drawable_size.x);
		float char_start_y = y_start + y_offset + slot->bitmap_top;
		char_start_y = ScreenToAnchor(char_start_y, drawable_size.y);

		float char_end_x = char_start_x + bitmap.width * 2.0f / drawable_size.x;
		float char_end_y = char_start_y - bitmap.rows * 2.0f / drawable_size.y;

		//std::cout << "char: x:" << char_start_x << "; y: " << char_start_y << "\n";

		//std::cout << "char width: " << char_end_x - char_start_x << "\n";
		//std::cout << "char height: " << char_end_y - char_start_y << "\n";

		ShapeTextureProgram::Vertex vertexes[]{
		{{char_start_x, char_start_y}, color, {0, 0}},
		{{char_end_x, char_start_y}, color, {1, 0}},
		{{char_start_x, char_end_y}, color, {0, 1}},
		{{char_end_x, char_end_y}, color, {1, 1}}};

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shape_texture_program->DrawFont(vertexes, glyph_map[text[i]]);

		x_start += x_advance;
		y_start += y_advance;

		//std::cout << "Current span: " << x_start - x_origin << "\n";

	}
}

void FontSource::SetText(const std::string& text) 
{
	ClearText();

	hb_font = hb_ft_font_create(ft_face, nullptr);
	hb_buffer = hb_buffer_create();
	hb_buffer_add_utf8(hb_buffer, text.c_str(), -1, 0, -1);

	hb_buffer_set_direction(hb_buffer, HB_DIRECTION_LTR);
	hb_buffer_set_script(hb_buffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(hb_buffer, hb_language_from_string("en", -1));
	hb_shape(hb_font, hb_buffer, nullptr, 0);
	glyph_info = hb_buffer_get_glyph_infos(hb_buffer, nullptr);
	glyph_pos = hb_buffer_get_glyph_positions(hb_buffer, nullptr);

}

void FontSource::ClearText() 
{
	if (hb_buffer)
	{
		hb_buffer_destroy(hb_buffer);
		hb_buffer = nullptr;
	}

	if (hb_font)
	{
		hb_font_destroy(hb_font);
		hb_font = nullptr;
	}
}

void FontSource::ClearTextureMap()
{
	for (auto& pair : glyph_map)
	{
		shape_texture_program->DeleteTextureId(pair.second);
	}
}