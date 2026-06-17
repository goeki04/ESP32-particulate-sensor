import 'dart:io';
import 'dart:ui' show ImageFilter;
import 'package:flutter/material.dart';

class Project {
  final IconData icon;
  final String name;
  final String meta;
  final String? tag;
  final String path;
  const Project(this.icon, this.name, this.meta, {this.tag, this.path = ''});
}

const List<Project> _recent = [
  Project(Icons.view_in_ar_outlined, 'SpaceShooter',
      'v0.4.2 · vor 2 Stunden geändert',
      tag: 'aktiv', path: r'C:\Projects\SpaceShooter'),
  Project(Icons.forest_outlined, 'OpenWorld', 'v0.4.1 · gestern geändert',
      path: r'C:\Projects\OpenWorld'),
  Project(Icons.sports_martial_arts, 'DungeonRPG', 'v0.3.8 · vor 3 Wochen',
      tag: 'archiv', path: r'C:\Projects\DungeonRPG'),
];

const List<Project> _all = [
  Project(Icons.directions_car_filled_outlined, 'RacingPrototype',
      'v0.2.0 · vor 2 Monaten'),
  Project(Icons.extension_outlined, 'PuzzleEngine', 'v0.1.4 · vor 5 Monaten',
      tag: 'archiv'),
];

class ProjectList extends StatefulWidget {
  const ProjectList({super.key});

  @override
  State<ProjectList> createState() => _ProjectListState();
}

class _ProjectListState extends State<ProjectList> {
  final ScrollController _controller = ScrollController();

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        const _Hero(),
        Container(
          height: 0.5,
          margin: const EdgeInsets.symmetric(horizontal: 22),
          color: Colors.white.withValues(alpha: 0.05),
        ),
        Expanded(
          child: RawScrollbar(
            controller: _controller,
            thumbColor: Colors.white.withValues(alpha: 0.10),
            thickness: 3,
            radius: const Radius.circular(99),
            child: ListView(
              controller: _controller,
              padding: const EdgeInsets.fromLTRB(28, 14, 28, 20),
              children: [
                const _SectionLabel('Zuletzt geöffnet'),
                ..._recent.map((p) => _ProjectRow(project: p)),
                const SizedBox(height: 20),
                const _SectionLabel('Alle Projekte'),
                ..._all.map((p) => _ProjectRow(project: p)),
              ],
            ),
          ),
        ),
      ],
    );
  }
}

class _Hero extends StatelessWidget {
  const _Hero();

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(28, 28, 28, 20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'WILLKOMMEN ZURÜCK',
            style: TextStyle(
              fontSize: 10,
              letterSpacing: 1.8,
              color: Colors.white.withValues(alpha: 0.42),
            ),
          ),
          const SizedBox(height: 8),
          RichText(
            text: TextSpan(
              style: const TextStyle(
                fontSize: 26,
                fontWeight: FontWeight.w200,
                height: 1.2,
              ),
              children: [
                TextSpan(
                  text: 'Deine Projekte\n',
                  style: TextStyle(color: Colors.white.withValues(alpha: 0.82)),
                ),
                TextSpan(
                  text: '3 aktiv · heute gearbeitet',
                  style: TextStyle(
                    color: Colors.white.withValues(alpha: 0.55),
                    fontSize: 14,
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 16),
          Row(
            children: [
              _HeroButton(
                label: 'Neues Projekt',
                icon: Icons.add,
                filled: true,
                onTap: () {},
              ),
              const SizedBox(width: 8),
              _HeroButton(label: 'Importieren', filled: false, onTap: () {}),
            ],
          ),
        ],
      ),
    );
  }
}

class _HeroButton extends StatefulWidget {
  final String label;
  final IconData? icon;
  final bool filled;
  final VoidCallback onTap;
  const _HeroButton({
    required this.label,
    required this.filled,
    required this.onTap,
    this.icon,
  });

  @override
  State<_HeroButton> createState() => _HeroButtonState();
}

class _HeroButtonState extends State<_HeroButton> {
  bool _hover = false;

  @override
  Widget build(BuildContext context) {
    final filled = widget.filled;
    final bg = filled
        ? Colors.white.withValues(alpha: _hover ? 0.14 : 0.08)
        : Colors.transparent;
    final fgOpacity =
        filled ? (_hover ? 0.95 : 0.82) : (_hover ? 0.72 : 0.55);
    final borderOpacity =
        filled ? 0.20 : (_hover ? 0.14 : 0.08);

    return MouseRegion(
      cursor: SystemMouseCursors.click,
      onEnter: (_) => setState(() => _hover = true),
      onExit: (_) => setState(() => _hover = false),
      child: GestureDetector(
        onTap: widget.onTap,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          padding: const EdgeInsets.symmetric(horizontal: 18, vertical: 7),
          decoration: BoxDecoration(
            color: bg,
            borderRadius: BorderRadius.circular(99),
            border: Border.all(
                color: Colors.white.withValues(alpha: borderOpacity), width: 0.5),
          ),
          child: Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              if (widget.icon != null) ...[
                Icon(widget.icon,
                    size: 12, color: Colors.white.withValues(alpha: fgOpacity)),
                const SizedBox(width: 5),
              ],
              Text(
                widget.label,
                style: TextStyle(
                  fontSize: 11.5,
                  color: Colors.white.withValues(alpha: fgOpacity),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _SectionLabel extends StatelessWidget {
  final String text;
  const _SectionLabel(this.text);

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 10),
      child: Text(
        text.toUpperCase(),
        style: TextStyle(
          fontSize: 9.5,
          letterSpacing: 1.4,
          color: Colors.white.withValues(alpha: 0.38),
        ),
      ),
    );
  }
}

class _ProjectRow extends StatefulWidget {
  final Project project;
  const _ProjectRow({required this.project});

  @override
  State<_ProjectRow> createState() => _ProjectRowState();
}

class _ProjectRowState extends State<_ProjectRow> {
  bool _hover = false;

  Future<void> _launch() async {
    final p = widget.project;
    if (p.path.isEmpty) return;
    try {
      await Process.start(
        r'..\Andromeda\conan\build\Release\Andromeda.exe',
        ['-project', p.path],
      );
      // Optionally: exit(0);
    } catch (_) {
      // Engine binary not found — ignore so the launcher doesn't crash.
    }
  }

  @override
  Widget build(BuildContext context) {
    final p = widget.project;
    return Padding(
      padding: const EdgeInsets.only(bottom: 6),
      child: MouseRegion(
        cursor: SystemMouseCursors.click,
        onEnter: (_) => setState(() => _hover = true),
        onExit: (_) => setState(() => _hover = false),
        child: GestureDetector(
          onTap: _launch,
          child: AnimatedContainer(
            duration: const Duration(milliseconds: 150),
            transform: Matrix4.translationValues(_hover ? 2 : 0, 0, 0),
            child: ClipRRect(
              borderRadius: BorderRadius.circular(10),
              child: BackdropFilter(
                filter: ImageFilter.blur(sigmaX: 8, sigmaY: 8),
                child: Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 14, vertical: 11),
                  decoration: BoxDecoration(
                    color: Colors.white.withValues(alpha: _hover ? 0.08 : 0.04),
                    borderRadius: BorderRadius.circular(10),
                    border: Border.all(
                      color: Colors.white.withValues(alpha: _hover ? 0.15 : 0.07),
                      width: 0.5,
                    ),
                  ),
                  child: Row(
                    children: [
                      // Icon box
                      Container(
                        width: 34,
                        height: 34,
                        decoration: BoxDecoration(
                          color: Colors.white.withValues(alpha: 0.05),
                          borderRadius: BorderRadius.circular(8),
                          border: Border.all(
                              color: Colors.white.withValues(alpha: 0.08),
                              width: 0.5),
                        ),
                        child: Icon(p.icon,
                            size: 16, color: Colors.white.withValues(alpha: 0.62)),
                      ),
                      const SizedBox(width: 14),
                      // Name + meta
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            Row(
                              children: [
                                Flexible(
                                  child: Text(
                                    p.name,
                                    overflow: TextOverflow.ellipsis,
                                    style: TextStyle(
                                      fontSize: 12.5,
                                      color: Colors.white.withValues(alpha: 0.88),
                                    ),
                                  ),
                                ),
                                if (p.tag != null) ...[
                                  const SizedBox(width: 6),
                                  _Tag(p.tag!),
                                ],
                              ],
                            ),
                            const SizedBox(height: 2),
                            Text(
                              p.meta,
                              style: TextStyle(
                                fontSize: 10.5,
                                color: Colors.white.withValues(alpha: 0.48),
                              ),
                            ),
                          ],
                        ),
                      ),
                      Icon(
                        Icons.chevron_right,
                        size: 16,
                        color: Colors.white.withValues(alpha: _hover ? 0.65 : 0.32),
                      ),
                    ],
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _Tag extends StatelessWidget {
  final String text;
  const _Tag(this.text);

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 7, vertical: 1),
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(99),
        border: Border.all(color: Colors.white.withOpacity(0.10), width: 0.5),
      ),
      child: Text(
        text,
        style: TextStyle(fontSize: 9, color: Colors.white.withOpacity(0.52)),
      ),
    );
  }
}
